#include <iostream>
#include <cstdint>
#include <cmath>
#include <algorithm>

#include "tiny.hpp"

void line(int32_t x0, int32_t y0, int32_t x1, int32_t y1,
          tiny::image &image, tiny::color const& color) {
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
        if (steep) image.set(y, x, color);
        else image.set(x, y, color);
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

    const int32_t WIDTH = 1024;
    const int32_t HEIGHT = 1024;

    tiny::image image(WIDTH, HEIGHT);
    tiny::model model("assets/suzanne.obj");

    for (int32_t i = 0; i < model.nfaces(); i++) {
        auto face = model.face(i);
        for (int32_t j = 0; j < 3; j++) {
            auto v0 = model.vert(face[j]);
            auto v1 = model.vert(face[(j + 1) % 3]);
            int32_t x0 = (v0.x + 1.f) * WIDTH  / 2.f;
            int32_t y0 = (v0.y + 1.f) * HEIGHT / 2.f;
            int32_t x1 = (v1.x + 1.f) * WIDTH  / 2.f;
            int32_t y1 = (v1.y + 1.f) * HEIGHT / 2.f;
            line(x0, y0, x1, y1, image, red);
        }
    }

    image.flipv();
    image.write_png("lesson2.png");

    std::cout << model << "\n";
    std::cout << image.get_color(10, 10) << '\n';
    std::cout << image << "\n";
    return 0;
}

