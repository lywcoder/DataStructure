#ifndef queueEmpty_
#define queueEmpty_

#include <iostream>
#include <string>

class queueEmpty {
public:
    queueEmpty(std::string theMessage = "Queue is empty") : message(theMessage) {}
    void outputMessage() { std::cout << message << std::endl; }
private:
    std::string message;
};

#endif
