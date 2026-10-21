#include <iostream>
#include "currency.h"

int main() {
    currency g, h(signType::plus, 3, 50), i, j;

    g.setValue(signType::minus, 2, 25);
    i.setValue(-6.45);

    j = h.add(g);
    h.output();
    std::cout << "+";
    g.output();
    std::cout << " = ";
    j.output();
    std::cout << std::endl;

    j = i.add(g).add(h);

    j = i.increment(g).add(h);

    std::cout << "Attemping to initialize with cents = 152" << std::endl;
}
