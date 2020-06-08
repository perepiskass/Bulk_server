#include "myasync.h"
#include "data.h"
#include <cstdlib>
#include <thread>
#include <chrono>
#include <future>
#include <memory>

/**
 * @brief Класс соответсвия указателя с индексом на него
 */
class MapData
{
    private:
    size_t count = 1;
    std::map <const size_t,std::array<int,3>> m;
    MapData(){}
    std::mutex log_mtx;

    public:
    static MapData& getInstance()
    {
        static MapData mapdata;
        return mapdata;
    }
};

namespace async 
{

void setCommands(DataIn* _handle,std::string&& str)
{
    std::lock_guard<std::mutex> l{_handle->mtx_input};
    const std::string delim = "\n";
    std::string::size_type start = 0;
    std::string::size_type end = 0;
    do
    {
        start = str.find_first_not_of(delim,start);
        end = str.find_first_of(delim,start);
        _handle->setData(std::forward<std::string>(str.substr(start,end-start)));

        start = end;
    }
    while(start != std::string::npos && end != str.size() -1);
}

handle_t connect(std::size_t bulk) 
{
    static std::atomic<size_t> i;
    auto bulkPtr =std::make_shared<DataIn>( new DataIn(bulk));
    std::weak_ptr<DataToConsole> cmdPtr = std::make_shared<DataToConsole>(new DataToConsole(bulkPtr));
    std::weak_ptr<DataToFile> filePtr = std::make_shared<DataToFile>(new DataToFile(bulkPtr));
    ++i;
    bulkPtr->vec_thread.emplace_back(new std::thread ([cmdPtr](){cmdPtr.lock()->update(i);}));
    ++i;
    bulkPtr->vec_thread.emplace_back(new std::thread ([filePtr](){filePtr.lock()->update(i);}));
    ++i;
    bulkPtr->vec_thread.emplace_back(new std::thread ([filePtr](){filePtr.lock()->update(i);}));

    Logger::getInstance().setCount(i);

    return reinterpret_cast<void*>(bulkPtr);
}

void receive(handle_t handle,const char *data,std::size_t) 
{
    auto _handle = reinterpret_cast<DataIn*>(handle);
    auto as(std::async(std::launch::async,setCommands, _handle, std::move(std::string(data))));
}

void write(handle_t handle)
{
    auto _handle = reinterpret_cast<DataIn*>(handle);
    _handle->write();
}

void disconnect(handle_t handle) 
{
    auto _handle = reinterpret_cast<DataIn*>(handle);
    _handle->works = false;
    _handle->write();
    for(auto& i : _handle->vec_thread)
    {
        i->join();
    }
    delete _handle;
}
}

namespace logger
{
    void printLog()
    {
        Logger::getInstance().print();
    }
}