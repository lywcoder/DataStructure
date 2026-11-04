#include "currency/currency.h"

#include <cassert>
#include <cmath>
#include <limits>
#include <sstream>
#include <stdexcept>

namespace {
void expectAmount(const currency& amount, signType sign,
                  unsigned long dollars, unsigned int cents) {
    assert(amount.getSign() == sign);
    assert(amount.getDollars() == dollars);
    assert(amount.getCents() == cents);
}

void testSignedArithmetic() {
    const currency positive(signType::plus, 3, 50);
    const currency negative(signType::minus, 2, 25);
    expectAmount(positive.add(negative), signType::plus, 1, 25);
    expectAmount(negative.add(positive), signType::plus, 1, 25);
    expectAmount(currency(signType::minus, 3, 10).add(positive),
                 signType::plus, 0, 40);
    expectAmount(currency(signType::plus, 1, 5).add(negative),
                 signType::minus, 1, 20);
    expectAmount(currency(signType::minus, 1, 80).add(negative),
                 signType::minus, 4, 5);
    expectAmount(positive.add(currency(signType::minus, 3, 50)),
                 signType::plus, 0, 0);
    currency value = negative;
    assert(&value.increment(positive) == &value);
    expectAmount(value, signType::plus, 1, 25);
    value.increment(value);
    expectAmount(value, signType::plus, 2, 50);
    expectAmount(negative, signType::minus, 2, 25);
}

void testRoundingAndValidation() {
    currency value;
    value.setValue(-6.45);
    expectAmount(value, signType::minus, 6, 45);
    value.setValue(1.125); // Exactly representable; a half cent rounds away from zero.
    expectAmount(value, signType::plus, 1, 13);
    value.setValue(-1.125);
    expectAmount(value, signType::minus, 1, 13);
    value.setValue(1.9999);
    expectAmount(value, signType::plus, 2, 0);
    value.setValue(-0.001);
    expectAmount(value, signType::plus, 0, 0);
    expectAmount(currency(signType::minus, 0, 0), signType::plus, 0, 0);
    bool invalidCents = false;
    try { value.setValue(signType::plus, 2, 100); }
    catch (const illegalParameterValue&) { invalidCents = true; }
    assert(invalidCents);
    expectAmount(value, signType::plus, 0, 0);
    const double invalid[] = {std::numeric_limits<double>::infinity(),
                              std::numeric_limits<double>::quiet_NaN(),
                              std::numeric_limits<double>::max()};
    for (double amount : invalid) {
        bool rejected = false;
        try { value.setValue(amount); }
        catch (const illegalParameterValue&) { rejected = true; }
        assert(rejected);
        expectAmount(value, signType::plus, 0, 0);
    }
}

void testOverflowAndFormatting() {
    const unsigned long maximum = std::numeric_limits<unsigned long>::max();
    currency value(signType::plus, maximum, 99);
    bool overflow = false;
    try { value.increment(currency(signType::plus, 0, 1)); }
    catch (const std::overflow_error&) { overflow = true; }
    assert(overflow);
    expectAmount(value, signType::plus, maximum, 99);
    overflow = false;
    try { value.add(currency(signType::plus, 1, 0)); }
    catch (const std::overflow_error&) { overflow = true; }
    assert(overflow);
    expectAmount(value.add(currency(signType::minus, maximum, 98)),
                 signType::plus, 0, 1);
    std::ostringstream output;
    std::streambuf* previous = std::cout.rdbuf(output.rdbuf());
    currency(signType::minus, 0, 5).output();
    std::cout.rdbuf(previous);
    assert(output.str() == "-$0.05");
}
} // namespace

int main() {
    testSignedArithmetic();
    testRoundingAndValidation();
    testOverflowAndFormatting();
}
