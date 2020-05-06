#include "data.h"


//-----Data input methods----------------------------------------------------------------------
    DataIn::DataIn(int count):count(count),countTry(count)
    {
        bulk = nullptr;
        works = true;
    }

    DataIn::~DataIn()
    {
        Writer{} << "DataIn - destructor-start" << std::endl;
        for(auto&i : vec_thread)
        {
            delete i;
        }
        Writer{} << "DataIn - destructor-middle" << std::endl;
        for(auto& i : subs)
        {
            delete i;
        }
        Writer{} << "DataIn - destructor-end" << std::endl;
    }

    void DataIn::subscribe(Observer *obs)
    {
        subs.push_back(obs);
    }

    void DataIn::checkDilimiter(std::string& str)
    {

        if (str == "{")
        {
            // Writer{} << "It is { " << std::endl;
            if(checkD.first) ++checkD.second;
            else
            {
                // if(bulk.first.size())
                // {
                //     notify();
                //     bulk.first.clear();
                // }
                checkD.first = true;
                ++checkD.second;
            }
            
        }
        else if (str == "}")
        {
            if (checkD.second) --checkD.second;
        }
    }

    void DataIn::setData(std::string str) 
    {
        if (bulk == nullptr) bulk = new Bulk{};
        // std::scoped_lock sl{mtx_cmd,mtx_file};
        // Writer{} << "From setData - " << str << std::endl;
        // Logger::getInstance().set_lineCount(0);

        checkDilimiter(str);
        if(checkD.first)
        {
            if (str!="{" && str!="}")
            {
                if(bulk->first.size() == 0) 
                {
                    bulk->second = std::chrono::seconds(std::time(NULL));
                }
                // Writer{} << "push to bulk - " << str << std::endl;
                // Logger::getInstance().set_commandCount();
                bulk->first.emplace_back(std::forward<std::string>(str));
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
                // Writer{} << "push to bulk - " << str << std::endl;
                // Logger::getInstance().set_commandCount();
                bulk->first.emplace_back(std::forward<std::string>(str));
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
        // Logger::getInstance().set_bulkCount();
        Writer{} << "notify" << std::endl;
        if(bulk)
        {
            setQueues();
            cv.notify_all();
        }
    }

    void DataIn::clearData()
    {   
        // bulk.first.erase(std::begin(bulk.first),std::end(bulk.first));
        Writer{} << "clearData - " << std::endl;
        // if (bulk.first.empty()) Writer{} << "vector empty!" << std::endl;
        delete bulk;
        bulk = nullptr;
        checkD.first = false;
        checkD.second = 0;
        countTry = count;
    }

    void DataIn::setQueues()
    {
        for(auto& i : subs)
        {
            Writer{} << "send bulk " << bulk->first.front() << std::endl;
            i->setBulk(*bulk);
        }
    }

//-----Data to console methods-------------------------------------------------------------------
    DataToConsole::DataToConsole(DataIn* data):_data(data)
    {
        data->subscribe(this);
    }

    DataToConsole::~DataToConsole()
    {
        Writer{} << "DataToConsole dest" << std::endl;
    }

    void DataToConsole::setBulk(const Bulk& bulk)
    {
        std::lock_guard<std::mutex> l{_data->mtx_cmd};
        bulkQ.push(bulk);
        Writer{} << "setBulk - " << bulk.first.front() << std::endl;
    }

    void DataToConsole::update(int id)
    {
        // Writer{}<< "THREAD until cicle " << id << std::endl;
         while(_data->works || !bulkQ.empty())
        {
        // Writer{}<< "THREAD after cicle " << id << std::endl;
            std::unique_lock<std::mutex> consolLock(_data->mtx_cmd);
            _data->cv.wait(consolLock,[&](){
            if(!bulkQ.empty() || !_data->works) return true;
            else return false;
            });
        Writer{}<< "THREAD into WAIT " << bulkQ.front().first.front() << std::endl;

            while(!bulkQ.empty())
            {
        Writer{}<< "THREAD into while " << bulkQ.front().first.front() << std::endl;
                if(bulkQ.front().first.empty() ) break;
                // Logger::getInstance().set_bulkCount(id);
                Writer::console(bulkQ.front().first,id);
                bulkQ.pop();
            }
                
        }
    }

//-----Data to file methods-----------------------------------------------------------------------
    DataToFile::DataToFile(DataIn* data):_data(data)
    {
        data->subscribe(this);
    }

    DataToFile::~DataToFile()
    {
        Writer{} << "DataToFile dest" << std::endl;
    }

    void DataToFile::setBulk(const Bulk& bulk)
    {
        std::lock_guard<std::mutex> l{_data->mtx_file};
        bulkQ.push(bulk);
    }

    void DataToFile::update(int id)
    {
        while (_data->works || !bulkQ.empty())
        {
            std::unique_lock<std::mutex> fileLock(_data->mtx_file);
            _data->cv.wait(fileLock,[&](){
            if(!bulkQ.empty() || !_data->works) return true;
            else return false;
            });
                while (!bulkQ.empty())
                {
                    if(bulkQ.front().first.empty() ) break;
                    auto start(std::chrono::steady_clock::now());
                    // Logger::getInstance().set_bulkCount(id);
                    Writer::file(bulkQ.front(),id,start);
                    bulkQ.pop();
                }
        }
    }
