#pragma once
#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>
#include "data.h"

using boost::asio::ip::tcp;

class session: public std::enable_shared_from_this<session>
{
public:
  session(tcp::socket socket,DataIn* bulk);
  void start();

private:
  void do_read();

  tcp::socket socket_;
  enum { max_length = 1024 };
  char data_[max_length];
  DataIn* bulk_;
};

class server
{
public:
    server(boost::asio::io_service& io_service, size_t port, DataIn* bulk);
private:
  void do_accept(DataIn* bulk);


  tcp::acceptor acceptor_;
  tcp::socket socket_;
  DataIn* bulk_;
};
