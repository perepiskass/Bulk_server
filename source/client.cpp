#include "version_lib.h"
#include <iostream>


int main(int argc, char *argv[]) 
{
  const std::string ver = "-version";
  if ( argc > 1)
  {
    if (argv[1] == ver)     std::cout << "version: " << version_major()<< '.'<< version_minor() << '.' << version_patch() << std::endl;
    else                    std::cout << "Type: -version, if you want to known proramm version" << std::endl;    
  }
  
    
  try
  {
    std::cout << "It is a client" << std::endl;
  }
  
  catch(const std::exception& e)
  {
    std::cerr << e.what() << '\n';
  }

    return 0;
}
