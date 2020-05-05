#pragma once
#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>
#include "myasync.h"
#include <string>

using boost::asio::ip::tcp;

class bulk
{
  public:
    void setCommand(const char* str);
    bulk(const async::handle_t& bulk);
    bool checkCommand(std::string data);
    bool checkSession();
    async::handle_t getServerBulk();
    async::handle_t getSessionBulk();


  private:
    size_t delimetr;
    async::handle_t bulk_server;
    async::handle_t bulk_session;
};

class session: public std::enable_shared_from_this<session>
{
public:
  session(tcp::socket socket,async::handle_t bulk);
  void start();

private:
  void do_read();

  tcp::socket socket_;
  enum { max_length = 1024 };
  char data_[max_length];
  bulk bulk_;
  static size_t count;
};

class server
{
public:
    server(boost::asio::io_service& io_service, std::pair<size_t,size_t> args);
    ~server();
private:
  void do_accept(async::handle_t bulk);


  tcp::acceptor acceptor_;
  tcp::socket socket_;
  async::handle_t bulk_;
};
