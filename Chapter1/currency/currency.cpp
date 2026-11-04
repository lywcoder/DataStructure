#include "currency.h"

#include <cmath>
#include <limits>
#include <stdexcept>

void currency::setValue(signType theSign,unsigned long theDollars,
                                        unsigned int theCents) {
    if (theCents > 99) throw illegalParameterValue("Cents should be < 100");

    sign = (theDollars == 0 && theCents == 0) ? signType::plus : theSign;
    dollars = theDollars;
    cents = theCents;
}

void currency::setValue(double theAmount) {
    if (!std::isfinite(theAmount))
        throw illegalParameterValue("Amount must be finite");
    const long double magnitude = std::fabs(static_cast<long double>(theAmount));
    const unsigned long maximum = std::numeric_limits<unsigned long>::max();
    // Check before floating-to-integer conversion; out-of-range conversion is unsafe.
    if (magnitude >= static_cast<long double>(maximum) + 1.0L)
        throw illegalParameterValue("Amount is out of range");
    unsigned long newDollars = static_cast<unsigned long>(magnitude);
    unsigned int newCents = static_cast<unsigned int>(
        std::floor((magnitude - newDollars) * 100.0L + 0.5L));
    if (newCents == 100) {
        if (newDollars == maximum)
            throw illegalParameterValue("Rounded amount is out of range");
        ++newDollars;
        newCents = 0;
    }
    setValue(theAmount < 0 ? signType::minus : signType::plus,
             newDollars, newCents);
}

// 构造函数实现
currency::currency(signType theSign, unsigned long theDollars, unsigned int theCents) {
    setValue(theSign, theDollars, theCents);
}

// add 方法实现
currency currency::add(const currency& x) const {
    if (sign == x.sign) {
        const unsigned long maximum = std::numeric_limits<unsigned long>::max();
        if (x.dollars > maximum - dollars)
            throw std::overflow_error("Currency addition overflow");
        unsigned long newDollars = dollars + x.dollars;
        const unsigned int totalCents = cents + x.cents;
        if (totalCents >= 100) {
            if (newDollars == maximum)
                throw std::overflow_error("Currency addition overflow");
            ++newDollars;
        }
        return currency(sign, newDollars, totalCents % 100);
    }

    // Different signs: subtract the smaller magnitude from the larger.
    const bool thisIsLarger = dollars > x.dollars ||
                             (dollars == x.dollars && cents >= x.cents);
    const currency& larger = thisIsLarger ? *this : x;
    const currency& smaller = thisIsLarger ? x : *this;
    unsigned long newDollars = larger.dollars - smaller.dollars;
    unsigned int newCents;
    if (larger.cents < smaller.cents) {
        --newDollars; // The magnitude comparison guarantees a dollar to borrow.
        newCents = larger.cents + 100 - smaller.cents;
    } else {
        newCents = larger.cents - smaller.cents;
    }
    return currency(larger.sign, newDollars, newCents);
}

// increment 方法实现
currency& currency::increment(const currency& x) {
    // Compute first: self-addition works and overflow leaves this object unchanged.
    *this = add(x);
    return *this;
}

// output 方法实现
void currency::output() const {
    if (sign == signType::minus) std::cout << '-';
    std::cout << '$' << dollars << '.';
    if (cents < 10) std::cout << '0';
    std::cout << cents;
}
