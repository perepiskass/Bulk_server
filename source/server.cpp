#include "version_lib.h"
#include "asio_async_server.h"
#include <csignal>
#include <iostream>

std::pair<size_t,size_t> checkArg(int argc,char** argv);
server* serverPtr;

void signalFunction(const boost::system::error_code & err, int signal)
{
  if(!err)
  {
    std::cout << std::endl;
    if (signal == 2)
    {
      serverPtr->~server();
      delete serverPtr;
    }
    exit(0);
  }
}

int main(int argc, char *argv[]) 
{

  auto args = checkArg(argc,argv);
    
  try
  {
    // signal(SIGINT,signalFunction);

    DataIn bulk(args.second);
    DataToConsole console(&bulk);
    DataToFile file(&bulk);

    boost::asio::io_service io_service;

    boost::asio::signal_set sig(io_service, SIGINT, SIGTERM);
    sig.async_wait(signalFunction);

    serverPtr = new server(io_service, args.first, &bulk);
    io_service.run();

    std::cout << "It is a server" << std::endl;
  }
  
  catch(const std::exception& e)
  {
    std::cerr << e.what() << '\n';
  }

    return 0;
}