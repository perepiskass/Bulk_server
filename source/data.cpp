#include "data.h"



//-----Data input methods----------------------------------------------------------------------
    DataIn::DataIn(int count):count(count),countTry(count)
    {}

    void DataIn::subscribe(Observer *obs)
    {
        subs.push_back(obs);
    }

    void DataIn::checkDilimiter(const std::string& str)
    {
        if (str == "{")
        {
            if(checkD.first) ++checkD.second;
            else
            {
                if(bulk.first.size())
                {
                    notify();
                    bulk.first.clear();
                }
                checkD.first = true;
                ++checkD.second;
            }
            
        }
        else if (str == "}")
        {
            if (checkD.second) --checkD.second;
        }
    }

    void DataIn::setData(std::string&& str) 
    {
        checkDilimiter(str);
        if(checkD.first)
        {
            if (str!="{" && str!="}")
            {
                if(bulk.first.size() == 0) 
                {
                    bulk.second = std::chrono::seconds(std::time(NULL));
                }
                bulk.first.emplace_back(std::forward<std::string>(str));
            }
            else if (!checkD.second)
            {
                notify();
                clearData();
            }
        }
        else
        {
            if (str!="{" && str!="}" && countTry)
            {
                if(bulk.first.size() == 0)
                {
                    bulk.second = std::chrono::seconds(std::time(NULL));
                }
                bulk.first.emplace_back(std::forward<std::string>(str));
                --countTry;
            }
            if(!countTry)
            {
                notify();
                clearData();
            }
        }
        
    }

    void DataIn::notify() 
    {
        for (auto s : subs) 
        {
            s->update(bulk);
        }
    }

    void DataIn::clearData()
    {
        bulk.first.clear();
        checkD.first = false;
        checkD.second = 0;
        countTry = count;
    }

    int DataIn::getQuantity()
    {
        return bulk.first.size();
    }


//-----Data to console methods-------------------------------------------------------------------
    DataToConsole::DataToConsole(DataIn *data) 
    {
        data->subscribe(this);
    }

    void DataToConsole::update(Bulk bulk)
    {
        std::cout << "bulk: ";
        for(auto str = bulk.first.begin(); str!=bulk.first.end(); ++str)
        {
            if(str==bulk.first.begin()) std::cout << *str;
            else std::cout << ", " << *str;
        }
        std::cout << std::endl;
    }


//-----Data to file methods-----------------------------------------------------------------------
    DataToFile::DataToFile(DataIn *data) 
    {
        data->subscribe(this);
    }

    void DataToFile::update(Bulk bulk)
    {
        std::ofstream out;
        auto timeUNIX = bulk.second.count();
        std::string path = "bulk"+ std::to_string(timeUNIX) + ".log";
        out.open(path);
        if (out.is_open(),std::ios::app)
        {
            out << "bulk: ";
            for(auto str = bulk.first.begin(); str!=bulk.first.end(); ++str)
            {
                if(str==bulk.first.begin()) out << *str;
                else out << ", " << *str;
            }
        }
        out.close();
    }
