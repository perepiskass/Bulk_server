#pragma once
#include <map>
#include <array>
#include <iostream>

/**
 * @brief Класс для сбора и вывода лог данных о количестве команд, групп сформированных в разных потоках
 */
class Logger
{
    private:
    size_t count = 1;
    std::map <const size_t,std::array<int,3>> m;
    Logger(){}
    std::mutex log_mtx;

    public:
    static Logger& getInstance()
    {
        static Logger logger;
        return logger;
    }

    void setCount(size_t c)
    {
        std::lock_guard<std::mutex> l{log_mtx};
        count = c;
    }

    void set_lineCount(size_t id = 0)
    {
        std::lock_guard<std::mutex> l{log_mtx};
        ++(m[id][0]);
    }

    void set_commandCount(size_t id = 0)
    {
        std::lock_guard<std::mutex> l{log_mtx};
        ++(m[id][1]);
    }   

    void set_bulkCount(size_t id = 0)
    {
        std::lock_guard<std::mutex> l{log_mtx};
        ++(m[id][2]);
    } 

    void print()
    {
        std::lock_guard<std::mutex> l{log_mtx};
        std::cout << "Main thread: "    << m[0][0]<< "- lines," 
                        << m[0][1]<< "- commands,"
                        << m[0][2]<< "- bulks" << std::endl;

        for (size_t i = 1; i <= count;++i)
        {
            std::cout << "Thread # " << i <<": "
                                    << m[i][1]<< "- commands,"
                                    << m[i][2]<< "- bulks" << std::endl;
        }
    }

     ~Logger(){}
};