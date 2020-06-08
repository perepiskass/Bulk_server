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
    size_t start_size = 0;
    std::map <const size_t,std::shared_ptr<DataIn>> m;
    MapData(){}
    std::mutex _mtx;

    public:
    static MapData& getInstance()
    {
        static MapData mapdata;
        return mapdata;
    }

    async::handle_t getHandle(std::shared_ptr<DataIn> ptr)
    {
        std::lock_guard<std::mutex> l{_mtx};
        m[++start_size] = ptr;
        return start_size;
    }

    std::shared_ptr<DataIn> getPtr(async::handle_t _handle)
    {
        std::lock_guard<std::mutex> l{_mtx};
        if(m.find(_handle)!=m.end())
        {
            return m.at(_handle);
        }
        return nullptr;
    }

    std::shared_ptr<DataIn> removePtr(async::handle_t _handle)
    {
        std::lock_guard<std::mutex> l{_mtx};
        if(m.find(_handle)!=m.end())
        {
            auto temp_ptr = m.at(_handle);
            m.erase(_handle);
            return temp_ptr;
        }
        return nullptr;
    }
};

namespace async 
{

void setCommands(std::shared_ptr<DataIn> _handle,std::string&& str)
{
    std::lock_guard<std::mutex> l{_handle->mtx_input};
    _handle->setData(std::forward<std::string>(str));
}

handle_t connect(const size_t bulk) 
{
    static std::atomic<size_t> i;
    std::shared_ptr<DataIn> bulkPtr{new DataIn(bulk)};
    auto cmdPtr = new DataToConsole(bulkPtr);
    auto filePtr = new DataToFile(bulkPtr);
    ++i;
    bulkPtr->vec_thread.emplace_back(new std::thread ([cmdPtr](){cmdPtr->update(i);}));
    ++i;
    bulkPtr->vec_thread.emplace_back(new std::thread ([filePtr](){filePtr->update(i);}));
    ++i;
    bulkPtr->vec_thread.emplace_back(new std::thread ([filePtr](){filePtr->update(i);}));

    Logger::getInstance().setCount(i);

    return MapData::getInstance().getHandle(bulkPtr);
}

void receive(handle_t handle,const char *data,std::size_t) 
{
    auto _handle = MapData::getInstance().getPtr(handle);
    auto as(std::async(setCommands, _handle, std::move(std::string(data))));
}

void write(handle_t handle)
{
    auto _handle = MapData::getInstance().getPtr(handle);
    _handle->write();
}

void disconnect(handle_t handle) 
{
    auto _handle = MapData::getInstance().removePtr(handle);
    _handle->works = false;
    _handle->write();
    for(auto& i : _handle->vec_thread)
    {
        i->join();
    }
}
}

namespace logger
{
    void printLog()
    {
        Logger::getInstance().print();
    }
}