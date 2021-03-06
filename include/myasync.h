#pragma once

#include <cstddef>

namespace async 
{

    using handle_t = size_t;

    handle_t connect(const std::size_t bulk);
    void receive(handle_t handle,const char *data,std::size_t);
    void disconnect(handle_t handle);
    void write(handle_t handle);

}
namespace logger
{
    void printLog();
}