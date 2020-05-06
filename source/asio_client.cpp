#include "asio_client.h"

client::client(): socket_(io_context)
{
}

void client::connect(ba::ip::address& addres, size_t port)
{
    ep = ba::ip::tcp::endpoint(addres,port);
    socket_.connect(ep);
}

void client::disconnect()
{
    socket_.close();
}

void client::write(std::string&& str)
{
    ba::write(socket_, ba::buffer(std::forward<std::string>(str), str.size()) );
}


    

