#pragma once
#include <boost/asio.hpp>
#include <iostream>

namespace ba = boost::asio;

class client
{
    public:
        client();
        void connect(std::string& addres, size_t port);
        void disconnect();
        void write(const char* str);
    private:
        ba::io_service io_context;
        ba::ip::tcp::endpoint ep;
        ba::ip::tcp::socket socket_;
};
    

