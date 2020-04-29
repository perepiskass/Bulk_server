#include "asio_client.h"

client::client(): socket_(io_context)
{
}

void client::connect(std::string& addres, size_t port)
{
    ep = ba::ip::tcp::endpoint(ba::ip::address::from_string(addres),port);
    socket_.connect(ep);
}

void client::disconnect()
{
    socket_.close();
}

void client::write(const char* str)
{
    auto lenght = strlen(str);
    ba::write(socket_, ba::buffer(str, lenght));
}


    

