#include "asio_async_server.h"

size_t session::count = 0;

//-----session-----------------------------------------------------------
session::session(tcp::socket socket,DataIn* bulk): socket_(std::move(socket)), bulk_(bulk)
  {
    session::count++;
  }

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
        if ( ec!=boost::asio::error::eof )
        {
          char* copy =(char*) std::malloc(length);
          std::strcpy(copy,data_);
          // if(std::string(copy) == "{") std::cout << "It is { " << std::endl;
          // std::cout << length << " - length serve" << std::endl;
          bulk_->setData(std::move(copy));
          memset(data_,0,sizeof(data_));
          do_read();
        }
        else 
        {
          std::cout<< "socket closed\n";
          session::count--;
          if(session::count == 0) bulk_->stop();
        }
      });
}


      // size_t up_to_enter(const boost::system::error_code &, std::size_t length) 
      // {
      //   for ( size_t i = 0; i < length; ++i)
      //     if ( data_[i] == '\n')
      //       return 0;
      //   return 1;
      // }


//-----server-----------------------------------------------------------
server::server(boost::asio::io_service& io_service, size_t port, DataIn* bulk)
  : acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
    socket_(io_service),bulk_(bulk)
{
  do_accept(bulk);
}

server::~server()
{
  bulk_->~DataIn();
  acceptor_.close();
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


