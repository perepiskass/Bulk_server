#include "data.h"



//-----Data input methods----------------------------------------------------------------------
    DataIn::DataIn(int count):count(count),countTry(count)
    {}
    DataIn::~DataIn()
    {
        stop();
    }
    void DataIn::subscribe(Observer *obs)
    {
        subs.push_back(obs);
    }

    void DataIn::checkDilimiter(const std::string& str)
    {
        std::cout << str << " checkDilimiter" << std::endl;
        if (str == "{")
        {
            // std::cout<< "check delimetr" << std::endl;
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
        std::cout << str << " setData" << std::endl;
        const std::string delim = "\\n\n";
        std::string::size_type start = 0;
        std::string::size_type end = 0;
        if(end >= str.size()) end = str.size() - 1;
        while(start != std::string::npos && end != str.size() -1)   // \ntttt\nyyyy\nrrrrrr
        {
            start = str.find_first_not_of(delim,start);
            end = str.find_first_of(delim,start);
            setCommand(std::forward<std::string>(str.substr(start,end-start)));
            start = end;
        }
    }

void DataIn::setCommand(std::string&& str)
{
    std::cout << str << " setCommand" << std::endl;
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
    void DataIn::stop()
    {
        if(!bulk.first.empty())
        {
            notify();
            clearData();
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
        auto start(std::chrono::steady_clock::now());
        std::ofstream out;
        auto timeUNIX = bulk.second.count();
        auto end(std::chrono::steady_clock::now());
        using nanoseconds = std::chrono::duration<double,std::ratio<1,1'000'000'000>>;
        auto diff = nanoseconds(end - start).count();
        std::string path = "bulk"+ std::to_string(timeUNIX) + '.' + std::to_string(int(diff)) + ".log";
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
