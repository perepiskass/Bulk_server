#include "version_lib.h"
#include "asio_async_server.h"
#include <iostream>

std::pair<size_t,size_t> checkArg(int argc,char** argv);

int main(int argc, char *argv[]) 
{

  auto args = checkArg(argc,argv);
    
  try
  {
    DataIn bulk(args.second);
    DataToConsole console(&bulk);
    DataToFile file(&bulk);

    boost::asio::io_service io_service;
    server server(io_service, args.first, &bulk);
    io_service.run();

    std::cout << "It is a server" << std::endl;
  }
  
  catch(const std::exception& e)
  {
    std::cerr << e.what() << '\n';
  }

    return 0;
}
