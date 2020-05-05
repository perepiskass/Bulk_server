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
    // Writer{} << "From setCommands - " << str << std::endl;
    const std::string delim = "\\n\n";
    std::string::size_type start = 0;
    std::string::size_type end = 0;
    do
    {
        start = str.find_first_not_of(delim,start);
        end = str.find_first_of(delim,start);
        _handle->setData(std::forward<std::string>(str.substr(start,end-start)));
        // Writer{} << "From cicle while setCommands - " << str.substr(start,end-start) << std::endl;

        start = end;
    }
    while(start != std::string::npos && end != str.size() -1);
}

handle_t connect(std::size_t bulk) 
{
    static int i = 1;
    auto bulkPtr = new DataIn(bulk);
    auto cmdPtr = new DataToConsole(bulkPtr);
    auto filePtr = new DataToFile(bulkPtr);

    bulkPtr->vec_thread.emplace_back(new std::thread ([cmdPtr](){cmdPtr->update(i++);}));
    bulkPtr->vec_thread.emplace_back(new std::thread ([filePtr](){filePtr->update(i++);}));
    bulkPtr->vec_thread.emplace_back(new std::thread ([filePtr](){filePtr->update(i++);}));

    return reinterpret_cast<void*>(bulkPtr);
}

void receive(handle_t handle,const char *data,std::size_t) 
{
    // Writer{} << "From receive - " << data << std::endl;
    auto _handle = reinterpret_cast<DataIn*>(handle);
    auto as(std::async(std::launch::async,setCommands, _handle, std::move(std::string(data))));
    // Writer{} << "receive end " << size << std::endl;
}

void display(handle_t handle)
{
    auto _handle = reinterpret_cast<DataIn*>(handle);
    _handle->write();
}

void disconnect(handle_t handle) 
{
    // Writer{} << "disconnect start " << std::endl;
    auto _handle = reinterpret_cast<DataIn*>(handle);
    _handle->works = false;
    _handle->write();
    // Writer{} << "disconnect middle " << std::endl;
    for(auto& i : _handle->vec_thread)
    {
    // Writer{} << "until join " << std::endl;
        // std::this_thread::sleep_for(std::chrono::milliseconds(100));
        i->detach();
        // i->join();
        // std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // Writer{} << "after join " << std::endl;

    }
    // std::this_thread::sleep_for(std::chrono::milliseconds(500));
    // delete _handle;
}

}
