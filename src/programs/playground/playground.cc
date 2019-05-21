

#include <queue>
#include <string>
#include <memory>
#include <iostream>

#include "EventArrows.h"

int main() {
    ArrowT<void, double> x;
    x.output = 22.2;
    ArrowT<double, int> y;
    y.input = 33.3;
    y.output = 4;
    ArrowT<int, void> z;
    z.input = 1;
    x.connect(&y);
    y.connect(&z);
    std::cout << "x.output = " << x.output << ", y.output = " << y.output << std::endl;

}



