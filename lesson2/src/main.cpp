#include <iostream>
#include <cstdint>
#include <cmath>
#include <algorithm>

#include "tiny.hpp"

tiny::color red(255, 0, 0);
tiny::color green(0, 255, 0);
tiny::color blue(0, 0, 255);
tiny::color white(0xFFFFFF);

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

void line(tiny::vec2<int32_t> t0, tiny::vec2<int32_t> t1,
          tiny::image &image, tiny::color const& color) {
    line(t0.x, t0.y, t1.x, t1.y, image, color);
}

void triangle_1(tiny::vec2<int32_t> t0, tiny::vec2<int32_t> t1,
              tiny::vec2<int32_t> t2, tiny::image &image, tiny::color const& color) {
    line(t0, t1, image, color);
    line(t1, t2, image, color);
    line(t2, t0, image, color);
}

void triangle_2(tiny::vec2<int32_t> t0, tiny::vec2<int32_t> t1,
              tiny::vec2<int32_t> t2, tiny::image &image,
              tiny::color const& color) {
    if (t0.y > t1.y) std::swap(t0, t1);
    if (t0.y > t2.y) std::swap(t0, t2);
    if (t1.y > t2.y) std::swap(t1, t2);
    line(t0, t1, image, green);
    line(t1, t2, image, green);
    line(t2, t0, image, red);
}

void triangle_3(tiny::vec2<int32_t> t0, tiny::vec2<int32_t> t1,
              tiny::vec2<int32_t> t2, tiny::image &image,
              tiny::color const& color) {
    if (t0.y > t1.y) std::swap(t0, t1);
    if (t0.y > t2.y) std::swap(t0, t2);
    if (t1.y > t2.y) std::swap(t1, t2);
    int32_t total_height = t2.y - t0.y;
    for (int32_t y = t0.y; y <= t1.y; y++) {
        int32_t segment_height = t1.y - t0.y + 1;
        float alpha = (float)(y - t0.y) / total_height;
        float beta  = (float)(y - t0.y) / segment_height;
        tiny::vec2<int32_t> A = { 
            .x = int32_t(t0.x + (t2.x - t0.x) * alpha),
            .y = int32_t(t0.y + (t2.y - t0.y) * alpha)
        }; // t0 + (t2 - t0) * alpha
        tiny::vec2<int32_t> B = {
            .x = int32_t(t0.x + (t1.x - t0.x) * beta),
            .y = int32_t(t0.y + (t1.y - t0.y) * beta)
        }; // t0 + (t1 - t0) * beta
        image.set(A.x, y, red);
        image.set(B.x, y, green);
    }
}

void triangle_4(tiny::vec2<int32_t> t0, tiny::vec2<int32_t> t1,
              tiny::vec2<int32_t> t2, tiny::image &image,
              tiny::color const& color) {
    if (t0.y > t1.y) std::swap(t0, t1);
    if (t0.y > t2.y) std::swap(t0, t2);
    if (t1.y > t2.y) std::swap(t1, t2);
    int32_t total_height = t2.y - t0.y;
    for (int32_t y = t0.y; y <= t1.y; y++) {
        int32_t segment_height = t1.y - t0.y + 1;
        float alpha = (float)(y - t0.y) / total_height;
        float beta  = (float)(y - t0.y) / segment_height;
        tiny::vec2<int32_t> A = { 
            .x = int32_t(t0.x + (t2.x - t0.x) * alpha),
            .y = int32_t(t0.y + (t2.y - t0.y) * alpha)
        }; // t0 + (t2 - t0) * alpha
        tiny::vec2<int32_t> B = {
            .x = int32_t(t0.x + (t1.x - t0.x) * beta),
            .y = int32_t(t0.y + (t1.y - t0.y) * beta)
        }; // t0 + (t1 - t0) * beta
        if (A.x > B.x) std::swap(A, B);
        for (int32_t j = A.x; j < B.x; j++) image.set(j, y, color);
    }
    for (int32_t y = t1.y; y <= t2.y; y++) {
        int32_t segment_height = t2.y - t1.y + 1;
        float alpha = (float)(y - t0.y) / total_height;
        float beta  = (float)(y - t1.y) / segment_height;
        tiny::vec2<int32_t> A = { 
            .x = int32_t(t0.x + (t2.x - t0.x) * alpha),
            .y = int32_t(t0.y + (t2.y - t0.y) * alpha)
        }; // t0 + (t2 - t0) * alpha
        tiny::vec2<int32_t> B = {
            .x = int32_t(t1.x + (t2.x - t1.x) * beta),
            .y = int32_t(t1.y + (t2.y - t1.y) * beta)
        }; // t1 + (t2 - t1) * beta
        if (A.x > B.x) std::swap(A, B);
        for (int32_t j = A.x; j < B.x; j++) image.set(j, y, color);
    }
}

void triangle(tiny::vec2<int32_t> t0, tiny::vec2<int32_t> t1,
              tiny::vec2<int32_t> t2, tiny::image &image,
              tiny::color const& color) {
    if (t0.y == t1.y && t0.y == t2.y) return;
    if (t0.y > t1.y) std::swap(t0, t1);
    if (t0.y > t2.y) std::swap(t0, t2);
    if (t1.y > t2.y) std::swap(t1, t2);
    int32_t total_height = t2.y - t0.y;
    for (int32_t i = 0; i < total_height; i++) {
        bool second_half = i > t1.y - t0.y || t1.y == t0.y;
        int32_t segment_height = second_half ? t2.y - t1.y : t1.y - t0.y;
        float alpha = (float)i / total_height;
        float beta  = (float)(i - (second_half ? t1.y - t0.y : 0)) / segment_height;
        tiny::vec2<int32_t> A = { 
            .x = int32_t(t0.x + (t2.x - t0.x) * alpha),
            .y = int32_t(t0.y + (t2.y - t0.y) * alpha)
        }; // t0 + (t2 - t0) * alpha

        //tiny::vec2<int32_t> B = second_half ? t1 + (t2 - t1) * beta : t0 + (t1 - t0) * beta;
        tiny::vec2<int32_t> B;
        if (second_half) {
            B.x = int32_t(t1.x + (t2.x - t1.x) * beta);
            B.y = int32_t(t1.y + (t2.y - t1.y) * beta);
        } else {
            B.x = int32_t(t0.x + (t1.x - t0.x) * beta);
            B.y = int32_t(t0.y + (t1.y - t0.y) * beta);
        }
        if (A.x > B.x) std::swap(A, B);
        for (int32_t j = A.x; j <= B.x; j++) image.set(j, t0.y + i, color);
    }
}

int32_t main(int32_t argc, char const *argv[]) {

    const int32_t WIDTH = 192;
    const int32_t HEIGHT = 192;

    tiny::image image(WIDTH, HEIGHT);
    // tiny::model model("assets/suzanne.obj");

    tiny::vec2<int32_t> t0[3] = {
        { .x = 10, .y = 70  },
        { .x = 50, .y = 160 },
        { .x = 70, .y = 80  }
    };
    tiny::vec2<int32_t> t1[3] = {
        { .x = 180, .y = 50  },
        { .x = 150, .y = 1   },
        { .x = 70,  .y = 180 }
    };
    tiny::vec2<int32_t> t2[3] = {
        { .x = 180, .y = 150 },
        { .x = 120, .y = 160 },
        { .x = 130, .y = 180 }
    };

    triangle(t0[0], t0[1], t0[2], image, red);
    triangle(t1[0], t1[1], t1[2], image, white);
    triangle(t2[0], t2[1], t2[2], image, green);

    image.flipv();
    image.write_png("lesson2.png");

    // std::cout << model << "\n";
    std::cout << image.get_color(10, 10) << '\n';
    std::cout << image << "\n";
    return 0;
}

