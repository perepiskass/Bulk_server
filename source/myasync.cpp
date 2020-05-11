#include "myasync.h"
#include "data.h"
#include <cstdlib>
#include <thread>
#include <chrono>
#include <future>

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
    static size_t i = 0;
    auto bulkPtr = new DataIn(bulk);
    auto cmdPtr = new DataToConsole(bulkPtr);
    auto filePtr = new DataToFile(bulkPtr);
    ++i;
    bulkPtr->vec_thread.emplace_back(new std::thread ([cmdPtr](){cmdPtr->update(i);}));
    ++i;
    bulkPtr->vec_thread.emplace_back(new std::thread ([filePtr](){filePtr->update(i);}));
    ++i;
    bulkPtr->vec_thread.emplace_back(new std::thread ([filePtr](){filePtr->update(i);}));

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