#include "version_lib.h"
#include <iostream>
#include <string>
#include <utility>

/**
 * @brief Функция для проверки введенных аргументов программы.
 */
auto checkArg(int argc,char** argv)
{
    if ( argc > 1)
    {
        std::string version = "-version";
        if (argv[1] == version)
        {
            std::cout << "version: " << version_major()<< '.'<< version_minor() << '.' << version_patch() << std::endl;
            exit(0);
        }
        else
        {
            std::pair<std::string,size_t> result;
            result.first = argv[1];
            result.second = atoi(argv[2]);
            if (result.second > 0)  // regexpr
            {
                return result;
            } 
            else  
            {

                std::cout << "Enter correct address or port"<< std::endl;
                exit(0);
            }
        }
    }
    else 
    {
      std::cout << "Usage: async_tcp_echo_server <ip_address> <port> or -version" << std::endl;
      exit(0);
    }
    
}