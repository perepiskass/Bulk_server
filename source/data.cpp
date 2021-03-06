#include "data.h"


//-----Data input methods----------------------------------------------------------------------
    DataIn::DataIn(const size_t count):works(true),bulk(nullptr),count(count),countTry(count)
    {
    }

    DataIn::~DataIn()
    {
    }

    void DataIn::subscribe(Observer *obs)
    {
        subs.emplace_back(obs);
    }

    void DataIn::checkDilimiter(std::string& str)
    {

        if (str == "{")
        {
            if(checkD.first) ++checkD.second;
            else
            {
                if(bulk->first.size())
                {
                    notify();
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
        if(bulk == nullptr) bulk.reset(new Bulk{});
        Logger::getInstance().set_lineCount(0);

        checkDilimiter(str);
        if(checkD.first)
        {
            if (str!="{" && str!="}")
            {
                if(bulk->first.size() == 0) 
                {
                    bulk->second = std::chrono::seconds(std::time(NULL));
                }
                Logger::getInstance().set_commandCount();
                bulk->first.emplace_back(str);
            }
            else if (!checkD.second)
            {
                write();
            }
        }
        else
        {
            if (str!="{" && str!="}" && countTry)
            {
                if(bulk->first.size() == 0)
                {
                    bulk->second = std::chrono::seconds(std::time(NULL));
                }
                Logger::getInstance().set_commandCount();
                bulk->first.emplace_back(str);
                --countTry;
            }
            if(!countTry)
            {
                write();
            }
        }
        
    }

    void DataIn::write()
    {
        notify();
        clearData();
    }

    void DataIn::notify()
    {
        if(bulk != nullptr)
        {
            Logger::getInstance().set_bulkCount();
            setQueues();
            cv.notify_all();
        }
        else cv.notify_all();

        bulk = nullptr;
    }

    void DataIn::clearData()
    {   
        checkD.first = false;
        checkD.second = 0;
        countTry = count;
    }

    void DataIn::setQueues()
    {
        for(auto& i : subs)
        {
            i->setBulk(*bulk);
        }
    }

//-----Data to console methods-------------------------------------------------------------------
    DataToConsole::DataToConsole(std::shared_ptr<DataIn> data):_data(data)
    {
        _data.lock()->subscribe(this);
    }

    DataToConsole::~DataToConsole()
    {
    }

    void DataToConsole::setBulk(const Bulk& bulk)
    {
        std::lock_guard<std::mutex> l{_data.lock()->mtx_cmd};
        bulkQ.push(bulk);
    }

    void DataToConsole::update(size_t id)
    {
         while(_data.lock()->works || !bulkQ.empty())
        {
            std::unique_lock<std::mutex> consolLock(_data.lock()->mtx_cmd);
            _data.lock()->cv.wait(consolLock,[&](){
            if(!bulkQ.empty() || !_data.lock()->works) return true;
            else return false;
            });

            while(!bulkQ.empty())
            {
                if(!bulkQ.front().first.empty())
                {
                    Logger::getInstance().set_bulkCount(id);
                    Writer::console(bulkQ.front().first,id);
                }
                bulkQ.pop();
            }
                
        }
    }

//-----Data to file methods-----------------------------------------------------------------------
    DataToFile::DataToFile(std::shared_ptr<DataIn> data):_data(data)
    {
        _data.lock()->subscribe(this);
    }

    DataToFile::~DataToFile()
    {
    }

    void DataToFile::setBulk(const Bulk& bulk)
    {
        std::lock_guard<std::mutex> l{_data.lock()->mtx_file};
        bulkQ.push(bulk);
    }

    void DataToFile::update(size_t id)
    {
        while (_data.lock()->works || !bulkQ.empty())
        {
            std::unique_lock<std::mutex> fileLock(_data.lock()->mtx_file);
            _data.lock()->cv.wait(fileLock,[&](){
            if(!bulkQ.empty() || !_data.lock()->works) return true;
            else return false;
            });
                while (!bulkQ.empty())
                {
                    if(!bulkQ.front().first.empty())
                    {
                        auto start(std::chrono::steady_clock::now());
                        // Logger::getInstance().set_bulkCount(id);
                        Writer::file(bulkQ.front(),id,start);
                    }
                    bulkQ.pop();
                }
        }
    }
