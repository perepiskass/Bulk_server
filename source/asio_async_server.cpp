#include "asio_async_server.h"

size_t session::count = 0;

//-----bulk--------------------------------------------------------------
bulk::bulk(const async::handle_t& bulk):delimetr(0),bulk_server(bulk)
{

}
async::handle_t bulk::getServerBulk()
{
  return bulk_server;
}

async::handle_t bulk::getSessionBulk()
{
  return bulk_session;
}

bool bulk::checkSession()
{
  if (delimetr) return true;
  else return false;
}

void bulk::setCommand(const char* str)
{
  if(checkCommand(str))
  {
   async::receive(bulk_session,str,strlen(str));
   std::cout << "bulk_session - start " << str << std::endl;
  }
  else
  {
   async::receive(bulk_server,str,strlen(str));
   std::cout << "bulk_server - start " << str << std::endl;
  }
}

bool bulk::checkCommand(std::string data)
{
  std::cout << "checkCommand " << data << std::endl;
  if(data == "{")
  {
    if(delimetr) 
    {
      ++delimetr;
      return true;
    }
    else
    {
      std::cout << "connect start" << std::endl;
      bulk_session = async::connect(1);
      ++delimetr;
      std::cout << "display start" << std::endl;
      async::display(bulk_server);
      std::cout << "display end" << std::endl;
      return true;
    }
  }
  else if(data=="}")
  {
    if((delimetr-1)==0)
    {
      --delimetr;
      async::disconnect(bulk_session);
      bulk_session = nullptr;
      return false;
    }
    else if (!delimetr) return false;
    else
    {
      --delimetr;
      return true;
    }
  }
  else
  {
    if(delimetr) return true;
  }
  
  return false;
}


//-----session-----------------------------------------------------------
session::session(tcp::socket socket,async::handle_t bulk): socket_(std::move(socket)), bulk_(bulk)
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
      [this, self](boost::system::error_code ec, std::size_t)
      {
        if ( ec!=boost::asio::error::eof )
        {
          bulk_.setCommand(data_);
          memset(data_,0,sizeof(data_));
          do_read();
        }
        else 
        {
          std::cout<< "socket closed\n";
          session::count--;
          if(bulk_.checkSession()) async::disconnect(bulk_.getSessionBulk());
          if(session::count == 0) async::disconnect(bulk_.getServerBulk());
        }
      });
}

//-----server-----------------------------------------------------------
server::server(boost::asio::io_service& io_service, std::pair<size_t,size_t> args)
  : acceptor_(io_service, tcp::endpoint(tcp::v4(), args.first)),
    socket_(io_service),bulk_(async::connect(args.second))
{
  do_accept(bulk_);
}

server::~server()
{
  acceptor_.close();
}

void server::do_accept(async::handle_t bulk)
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


