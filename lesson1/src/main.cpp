#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>

#include "tiny.hpp"

void line_1(int32_t x0, int32_t y0, int32_t x1, int32_t y1, tiny::image &image,
            tiny::color const &color) {
    for (float t = 0.f; t < 1.f; t += .01f) {
        int32_t x = x0 + (x1 - x0) * t;
        int32_t y = y0 + (y1 - y0) * t;
        image.set(x, y, color);
    }
}

void line_2(int32_t x0, int32_t y0, int32_t x1, int32_t y1, tiny::image &image,
            tiny::color const &color) {
    for (int32_t x = x0; x <= x1; x++) {
        const auto t = float(x - x0) / float(x1 - x0);
        int32_t y = y0 * (1.f - t) + y1 * t;
        image.set(x, y, color);
    }
}

void line_3(int32_t x0, int32_t y0, int32_t x1, int32_t y1, tiny::image &image,
            tiny::color const &color) {
    bool steep = false;
    if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    for (int32_t x = x0; x <= x1; x++) {
        const float t = (x - x0) / float(x1 - x0);
        int32_t y = y0 * (1.f - t) + y1 * t;
        if (steep)
            image.set(y, x, color);  // if transposed, de-tranpose
        else
            image.set(x, y, color);
    }
}

void line_4(int32_t x0, int32_t y0, int32_t x1, int32_t y1, tiny::image &image,
            tiny::color const &color) {
    bool steep = false;
    if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int32_t dx = x1 - x0;
    int32_t dy = y1 - y0;
    float d_error = std::abs(dy / float(dx));
    float error = 0.f;
    int32_t y = y0;
    for (int32_t x = x0; x <= x1; x++) {
        if (steep)
            image.set(y, x, color);
        else
            image.set(x, y, color);
        error += d_error;
        if (error > .5f) {
            y += (y1 > y0 ? 1 : -1);
            error -= 1.f;
        }
    }
}

void line(int32_t x0, int32_t y0, int32_t x1, int32_t y1, tiny::image &image,
          tiny::color const &color) {
    bool steep = false;
    if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int32_t dx = x1 - x0;
    int32_t dy = y1 - y0;
    int32_t d_error2 = std::abs(dy) * 2;
    int32_t error2 = 0;
    int32_t y = y0;
    for (int32_t x = x0; x <= x1; x++) {
        if (steep)
            image.set(y, x, color);
        else
            image.set(x, y, color);
        error2 += d_error2;
        if (error2 > dx) {
            y += (y1 > y0 ? 1 : -1);
            error2 -= dx * 2;
        }
    }
}

int32_t main(int32_t argc, char const *argv[]) {
    tiny::color red(255, 0, 0);
    tiny::color green(0, 255, 0);
    tiny::color blue(0, 0, 255);
    tiny::color white(0xFFFFFF);

    tiny::image image(100, 100);

    line(13, 20, 80, 40, image, white);
    line(20, 13, 40, 80, image, red);
    line(80, 40, 13, 20, image, red);

    image.flipv();
    image.write_png("lesson1.png");

    std::cout << image.get_color(10, 10) << '\n';
    std::cout << image << "\n";
    return 0;
}
