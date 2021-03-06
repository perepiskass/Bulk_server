#include "version_lib.h"
#include <iostream>
#include <string>
#include <utility>

/**
 * @brief Функция для получения введенных аргументов программы порта и размера bulk.
 */
auto getArg(int argc,char** argv)
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
            std::pair<size_t,size_t> result;
            result.first = atoi(argv[1]);
            result.second = atoi(argv[2]);
            if (result.first > 0 && result.second > 0) 
            {
                return result;
            } 
            else  
            {

                std::cout << "Enter correct port or bulk size"<< std::endl;
                exit(0);
            }
        }
    }
    else 
    {
      std::cout << "Usage: async_tcp_echo_server <port> <bulk size> or -version" << std::endl;
      exit(0);
    }
    
}