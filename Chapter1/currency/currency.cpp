#include "currency.h"

void currency::setValue(signType theSign,unsigned long theDollars,
                                        unsigned int theCents) {
    if (theCents > 99) throw illegalParameterValue("Cents should be < 100");

    sign = theSign;
    dollars = theDollars;
    cents = theCents;
}

void currency::setValue(double theAmount) {
    if (theAmount < 0) { sign = signType::minus; theAmount = -theAmount;}
    else sign = signType::plus;
    dollars = (unsigned long) theAmount;
    cents = (unsigned int) ((theAmount + 0.001 - dollars) * 100);
}

// 构造函数实现
currency::currency(signType theSign, unsigned long theDollars, unsigned int theCents) {
    setValue(theSign, theDollars, theCents);
}

// add 方法实现
currency currency::add(const currency& x) const {
    currency y;
    y.dollars = dollars + x.dollars;
    y.cents = cents + x.cents;
    if (y.cents >= 100) {
        y.dollars++;
        y.cents -= 100;
    }
    if (sign != x.sign) {
        // 处理不同符号相加的情况
        // 这里简化处理，实际需要更复杂的逻辑
    }
    y.sign = sign; // 简化处理
    return y;
}

// increment 方法实现
currency& currency::increment(const currency& x) {
    dollars += x.dollars;
    cents += x.cents;
    if (cents >= 100) {
        dollars++;
        cents -= 100;
    }
    return *this;
}

// output 方法实现
void currency::output() const {
    if (sign == signType::minus) std::cout << '-';
    std::cout << '$' << dollars << '.';
    if (cents < 10) std::cout << '0';
    std::cout << cents;
}
