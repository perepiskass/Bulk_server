#include "asio_async_server.h"


//-----session-----------------------------------------------------------
session::session(tcp::socket socket,DataIn* bulk): socket_(std::move(socket)), bulk_(bulk)
  {}

void session::start()
{
  do_read();
}


void session::do_read()
{
  auto self(shared_from_this());
  socket_.async_read_some(boost::asio::buffer(data_, max_length),
      [this, self](boost::system::error_code ec, std::size_t length)
      {
        if (!ec)
        {
          const char* delim = "\n";
          char* copy =(char*) std::malloc(length);
          std::strcpy(copy,data_);
          auto ptr = std::strtok(copy,delim);
          while (ptr)
          {
              bulk_->setData(ptr);
              ptr = strtok(0,delim);
          }

          do_read();
        }
      });
}


//-----server-----------------------------------------------------------
server::server(boost::asio::io_service& io_service, size_t port, DataIn* bulk)
  : acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
    socket_(io_service),bulk_(bulk)
{
  do_accept(bulk);
}

void server::do_accept(DataIn* bulk)
{
  acceptor_.async_accept(socket_,
      [this,bulk](boost::system::error_code ec)
      {
        if (!ec)
        {
          std::make_shared<session>(std::move(socket_),bulk)->start();
        }

        do_accept(bulk);
      });
}


