#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <chrono>
#include <fstream>


using Time = std::chrono::seconds;
using Bulk = std::pair<std::vector<std::string>,Time>;

// Observer or Sibscriber
class Observer {
public:
    virtual void update(Bulk bulk) = 0;
};
using Subscrabers = std::vector<Observer*>;

// Publisher or Observable
class Publisher {
public:
    virtual void subscribe(Observer* obs) = 0;
};

class DataIn : public Publisher {
public:
    void subscribe(Observer *obs) override;
    void setData(std::string&& str);
    void notify();
    int getQuantity();
    DataIn(int count);
private:
    void checkDilimiter(const std::string& str);
    void clearData();

    std::pair<bool,uint8_t> checkD; ///< переменная для проверки использования знаков динамического разделения блоков "{" и "}" и хранения состояния о их кол-ве
    //decltype(std::chrono::seconds(std::time(NULL))) 
    Bulk bulk;
    Subscrabers subs;
    const int count;        ///< хранит информацию о размере блока, задаеться при запуске программы (инициализируеться в конструкторе)
    int countTry;           ///< оставшееся ко-во команд для ввода в блок для его формирования
};

class DataToConsole : public Observer {
public:
    DataToConsole(DataIn *data);
    void update( Bulk bulk) override ;
};

class DataToFile : public Observer {
public:
    DataToFile(DataIn *data);
    void update(Bulk bulk) override;
};

