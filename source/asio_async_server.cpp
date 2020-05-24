#include "asio_async_server.h"

size_t session::count = 0;

//-----bulk--------------------------------------------------------------
Handler::Handler(const async::handle_t& handle_):delimetr_count(0),handle_server(handle_)
{}
async::handle_t Handler::getServerHandle()const
{
  return handle_server;
}

async::handle_t Handler::getSessionHandle()const
{
  return handle_session;
}

bool Handler::checkSession()const
{
  if (delimetr_count) return true;
  else return false;
}

void Handler::setCommand(const char* str)
{
  if(checkCommand(str))
  {
   async::receive(handle_session,str,strlen(str));
  }
  else
  {
   async::receive(handle_server,str,strlen(str));
  }
}

bool Handler::checkCommand(std::string data)
{
  if(data == "{")
  {
    if(delimetr_count) 
    {
      ++delimetr_count;
      return true;
    }
    else
    {
      handle_session = async::connect(1);
      ++delimetr_count;
      async::write(handle_server);
      return true;
    }
  }
  else if(data=="}")
  {
    if((delimetr_count-1)==0)
    {
      --delimetr_count;
      async::disconnect(handle_session);

      handle_session = nullptr;
      return false;
    }
    else if (!delimetr_count) return false;
    else
    {
      --delimetr_count;
      return true;
    }
  }
  else
  {
    if(delimetr_count) return true;
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
          session::count--;
          if(bulk_.checkSession()) async::disconnect(bulk_.getSessionHandle());
          if(session::count == 0) async::write(bulk_.getServerHandle());
        }
      });
}

//-----server-----------------------------------------------------------
server::server(boost::asio::io_service& io_service, std::pair<size_t,size_t>& args)
  : acceptor_(io_service, tcp::endpoint(tcp::v4(), args.first)),
    socket_(io_service),bulk_(async::connect(args.second))
{
  do_accept(bulk_);
}

server::~server()
{
  acceptor_.close();
  async::disconnect(bulk_);
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


