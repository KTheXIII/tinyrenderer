#include <iostream>

#include "tiny.hpp"

int32_t main(int32_t argc, char const *argv[]) {
    std::cout << "Hello, World!\n";

    tiny::color red(255, 0, 0);
    tiny::color green(0, 255, 0);
    tiny::color blue(0, 0, 255);

    tiny::image image(100, 100);

    image.set(10, 10, red);
    image.set(11, 10, green);
    image.write_png("lesson0.png");

    std::cout << image.get_color(10, 10) << '\n';
    std::cout << image << "\n";
    return 0;
}
