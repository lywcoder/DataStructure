#include <iostream>
#include <exception>
#include <string>

// 定义异常类
class illegalParameterValue {
public:
    illegalParameterValue(std::string theMessage = "Illegal parameter value")
        : message(theMessage) {}
    void outputMessage() { std::cout << message << std::endl; }
private:
    std::string message;
};

// 使用作用域枚举避免与 std::plus/std::minus 冲突
enum class signType { plus, minus };

class currency {
public:
    currency(signType theSign = signType::plus,
            unsigned long theDollars = 0,
            unsigned int theCents = 0);

    ~currency() {}
    void setValue(signType, unsigned long, unsigned int);
    void setValue(double);
    signType getSign() const { return sign; }
    unsigned long getDollars() const { return dollars; }
    unsigned int getCents() const { return cents; }
    currency add(const currency&) const;
    currency& increment(const currency&);
    void output() const;
private:
    signType sign;
    unsigned long dollars;
    unsigned int cents;
};
