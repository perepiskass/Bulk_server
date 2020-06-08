#include "version_lib.h"
#include "asio_client.h"
#include <csignal>

std::pair<ba::ip::address,size_t> getArg(int argc,char** argv);

std::unique_ptr<client> clientPtr;

void signalFunction(int)
{
  std::cout << std::endl;
  exit(0);
}


int main(int argc, char *argv[]) 
{
  auto args = getArg(argc,argv);
  try
  {
    clientPtr.reset( new client{} );
    clientPtr->connect(args.first,args.second);

    signal(SIGINT,signalFunction);

    std::string str;
    while(getline(std::cin,str))
    {
      clientPtr->write(std::move(str));
    }
  }
  
  catch(const std::exception& e)
  {
    std::cerr << e.what() << '\n';
  }

    return 0;
}
