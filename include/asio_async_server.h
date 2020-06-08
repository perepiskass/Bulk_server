#pragma once
#include <cstdlib>
#include <memory>
#include <utility>
#include <boost/asio.hpp>
#include "myasync.h"
#include <string>

using boost::asio::ip::tcp;

class Handler
{
  public:
    Handler(const async::handle_t& handle_);
    void setCommand(std::string str);
    bool checkCommand(std::string data);
    bool checkSession()const;
    async::handle_t getServerHandle()const;
    async::handle_t getSessionHandle()const;

  private:
    size_t delimetr_count;
    async::handle_t handle_server;
    async::handle_t handle_session;
};

class session: public std::enable_shared_from_this<session>
{
  public:
    session(tcp::socket socket,async::handle_t bulk);
    void start();

  private:
    void do_read();

    tcp::socket socket_;
    char data_;
    std::string command;
    Handler bulk_;
    static size_t count;
};

class server
{
  public:
    server(boost::asio::io_service& io_service, std::pair<size_t,size_t>& args);
    ~server();
  private:
    void do_accept(async::handle_t bulk);

    tcp::acceptor acceptor_;
    tcp::socket socket_;
    async::handle_t bulk_;
};
