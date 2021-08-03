#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <array>
#include <vector>

#include "nlohmann/json.hpp"
#include "tiny.hpp"

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

void line(tiny::vec2<int32_t> t0, tiny::vec2<int32_t> t1, tiny::image &image,
          tiny::color const &color) {
    line(t0.x, t0.y, t1.x, t1.y, image, color);
}

void triangle_1(tiny::vec2<int32_t> t0, tiny::vec2<int32_t> t1,
                tiny::vec2<int32_t> t2, tiny::image &image,
                tiny::color const &color) {
    line(t0, t1, image, color);
    line(t1, t2, image, color);
    line(t2, t0, image, color);
}

void triangle_2(tiny::vec2<int32_t> t0, tiny::vec2<int32_t> t1,
                tiny::vec2<int32_t> t2, tiny::image &image,
                tiny::color const &color) {
    if (t0.y > t1.y) std::swap(t0, t1);
    if (t0.y > t2.y) std::swap(t0, t2);
    if (t1.y > t2.y) std::swap(t1, t2);
    line(t0, t1, image, tiny::color::green());
    line(t1, t2, image, tiny::color::green());
    line(t2, t0, image, tiny::color::red());
}

void triangle_3(tiny::vec2<int32_t> t0, tiny::vec2<int32_t> t1,
                tiny::vec2<int32_t> t2, tiny::image &image,
                tiny::color const &color) {
    if (t0.y > t1.y) std::swap(t0, t1);
    if (t0.y > t2.y) std::swap(t0, t2);
    if (t1.y > t2.y) std::swap(t1, t2);
    int32_t total_height = t2.y - t0.y;
    for (int32_t y = t0.y; y <= t1.y; y++) {
        int32_t segment_height = t1.y - t0.y + 1;
        float alpha = (float)(y - t0.y) / total_height;
        float beta = (float)(y - t0.y) / segment_height;
        tiny::vec2<int32_t> A = {
            .x = int32_t(t0.x + (t2.x - t0.x) * alpha),
            .y = int32_t(t0.y +
                         (t2.y - t0.y) * alpha)};  // t0 + (t2 - t0) * alpha
        tiny::vec2<int32_t> B = {
            .x = int32_t(t0.x + (t1.x - t0.x) * beta),
            .y =
                int32_t(t0.y + (t1.y - t0.y) * beta)};  // t0 + (t1 - t0) * beta
        image.set(A.x, y, tiny::color::red());
        image.set(B.x, y, tiny::color::green());
    }
}

void triangle_4(tiny::vec2<int32_t> t0, tiny::vec2<int32_t> t1,
                tiny::vec2<int32_t> t2, tiny::image &image,
                tiny::color const &color) {
    if (t0.y > t1.y) std::swap(t0, t1);
    if (t0.y > t2.y) std::swap(t0, t2);
    if (t1.y > t2.y) std::swap(t1, t2);
    int32_t total_height = t2.y - t0.y;
    for (int32_t y = t0.y; y <= t1.y; y++) {
        int32_t segment_height = t1.y - t0.y + 1;
        float alpha = (float)(y - t0.y) / total_height;
        float beta = (float)(y - t0.y) / segment_height;
        tiny::vec2<int32_t> A = {
            .x = int32_t(t0.x + (t2.x - t0.x) * alpha),
            .y = int32_t(t0.y +
                         (t2.y - t0.y) * alpha)};  // t0 + (t2 - t0) * alpha
        tiny::vec2<int32_t> B = {
            .x = int32_t(t0.x + (t1.x - t0.x) * beta),
            .y =
                int32_t(t0.y + (t1.y - t0.y) * beta)};  // t0 + (t1 - t0) * beta
        if (A.x > B.x) std::swap(A, B);
        for (int32_t j = A.x; j < B.x; j++) image.set(j, y, color);
    }
    for (int32_t y = t1.y; y <= t2.y; y++) {
        int32_t segment_height = t2.y - t1.y + 1;
        float alpha = (float)(y - t0.y) / total_height;
        float beta = (float)(y - t1.y) / segment_height;
        tiny::vec2<int32_t> A = {
            .x = int32_t(t0.x + (t2.x - t0.x) * alpha),
            .y = int32_t(t0.y +
                         (t2.y - t0.y) * alpha)};  // t0 + (t2 - t0) * alpha
        tiny::vec2<int32_t> B = {
            .x = int32_t(t1.x + (t2.x - t1.x) * beta),
            .y =
                int32_t(t1.y + (t2.y - t1.y) * beta)};  // t1 + (t2 - t1) * beta
        if (A.x > B.x) std::swap(A, B);
        for (int32_t j = A.x; j < B.x; j++) image.set(j, y, color);
    }
}

void triangle_5(tiny::vec2<int32_t> t0, tiny::vec2<int32_t> t1,
              tiny::vec2<int32_t> t2, tiny::image &image,
              tiny::color const &color) {
    if (t0.y == t1.y && t0.y == t2.y) return;
    if (t0.y > t1.y) std::swap(t0, t1);
    if (t0.y > t2.y) std::swap(t0, t2);
    if (t1.y > t2.y) std::swap(t1, t2);
    int32_t total_height = t2.y - t0.y;
    for (int32_t i = 0; i < total_height; i++) {
        bool second_half = i > t1.y - t0.y || t1.y == t0.y;
        int32_t segment_height = second_half ? t2.y - t1.y : t1.y - t0.y;
        float alpha = (float)i / total_height;
        float beta =
            (float)(i - (second_half ? t1.y - t0.y : 0)) / segment_height;
        tiny::vec2<int32_t> A = {
            .x = int32_t(t0.x + (t2.x - t0.x) * alpha),
            .y = int32_t(t0.y + (t2.y - t0.y) * alpha),
        };  // t0 + (t2 - t0) * alpha
        // tiny::vec2<int32_t> B = second_half ? t1 + (t2 - t1) * beta : t0 +
        // (t1 - t0) * beta;
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

tiny::vec3<float> barycentric(std::array<tiny::vec2<int32_t>, 3> const& pts,
                              tiny::vec2<int32_t> const& P) {
    tiny::vec3<float> u = tiny::math::cross<float>(
                {
                    float(pts[2].x - pts[0].x),
                    float(pts[1].x - pts[0].x),
                    float(pts[0].x - P.x),
                },
                {
                    float(pts[2].y - pts[0].y),
                    float(pts[1].y - pts[0].y),
                    float(pts[0].y - P.y),
                }
            );
    if (std::abs(u.z) < 1) return tiny::vec3<float>{-1.f, 1.f, 1.f};
    return {
        1.f - (u.x + u.y) / u.z,
        u.y / u.z,
        u.x / u.z
    };
}

void triangle(std::array<tiny::vec2<int32_t>, 3> const& pts, tiny::image &image,
              tiny::color const& color) {
    tiny::vec2<int32_t> bboxmin{image.get_width() - 1, image.get_height() - 1};
    tiny::vec2<int32_t> bboxmax{0, 0};
    tiny::vec2<int32_t> clamp{image.get_width() - 1, image.get_height() - 1};
    for (int32_t i = 0; i < pts.size(); i++) {
        bboxmin.x = std::max(0,       std::min(bboxmin.x, pts[i].x));
        bboxmax.x = std::min(clamp.x, std::max(bboxmax.x, pts[i].x));
        bboxmin.y = std::max(0,       std::min(bboxmin.y, pts[i].y));
        bboxmax.y = std::min(clamp.y, std::max(bboxmax.y, pts[i].y));
    }

    tiny::vec2<int32_t> P;
    for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
        for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {
            tiny::vec3<float> bc_screen = barycentric(pts, P);
            if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) continue;
            image.set(P.x, P.y, color);
        }
    }
}

int32_t main(int32_t argc, char const *argv[]) {
    constexpr int32_t WIDTH  = 512;
    constexpr int32_t HEIGHT = 512;

    tiny::image image(WIDTH, HEIGHT);
    tiny::model model("assets/suzanne.obj");
    // tiny::model model("assets/african_head.obj");
    tiny::vec3<float> light_dir{ 0, 0, -1 };

    for (int32_t i = 0; i < model.nfaces(); i++) {
        std::vector<int32_t> face = model.face(i);
        std::array<tiny::vec2<int32_t>, 3> screen_coords;
        std::array<tiny::vec3<float>,   3> world_coords;
        for (int32_t j = 0; j < screen_coords.size(); j++) {
            auto v = model.vert(face[j]);
            screen_coords[j] = tiny::vec2<int32_t>{
                int32_t((v.x + 1.) * WIDTH  / 2.),
                int32_t((v.y + 1.) * HEIGHT / 2.)
            };
            world_coords[j] = v;
        }
        auto n = tiny::math::cross(
                    world_coords[2] - world_coords[0],
                    world_coords[1] - world_coords[0]
                );
        n = tiny::math::normalise(n);
        auto intensity = n * light_dir;
        if (intensity > 0)
            triangle_3(screen_coords[0], screen_coords[1], screen_coords[2],
                       image,
                       tiny::color(
                           uint8_t(intensity * 255.),
                           uint8_t(intensity * 255.),
                           uint8_t(intensity * 255.)
                      ));
    }

    image.flipv();
    image.write_png("lesson2.png");

    using json = nlohmann::json;
    std::cout << json::parse(image.json()).dump(2) << "\n";
    return 0;
}

