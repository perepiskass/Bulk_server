#include "version_lib.h"
#include "asio_client.h"
#include <iostream>
#include <string>

std::pair<std::string,size_t> checkArg(int argc,char** argv);


int main(int argc, char *argv[]) 
{
  auto args = checkArg(argc,argv);
  try
  {
      client cli{};
      cli.connect(args.first,args.second);

      std::string str;
      while(getline(std::cin,str))
      {
        cli.write(std::move(str));
      }
  }
  
  catch(const std::exception& e)
  {
    std::cerr << e.what() << '\n';
  }

    return 0;
}
