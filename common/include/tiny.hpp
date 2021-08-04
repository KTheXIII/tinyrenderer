#pragma once

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>

#include "stb/stb_image_write.h"

namespace tiny {

// TODO: Write operator overload for the vec data types

template <typename T>
struct vec2 {
    union {
        struct {
            T x, y;
        };
        struct {
            T u, v;
        };
        T raw[2];
    };
    vec2<T> operator+(vec2<T> const& rhs) const {
        return vec2<T>{x + rhs.x, y + rhs.y};
    }
    vec2<T> operator+(T const& rhs) const {
        return vec2<T>{x + rhs, y + rhs};
    }
    vec2<T> operator-(vec2<T> const& rhs) const {
        return vec2<T>{x - rhs.x, y - rhs.y};
    }
    vec2<T> operator-(T const& rhs) const {
        return vec2<T>{x - rhs, y - rhs};
    }
    vec2<T> operator*(vec2<T> const& rhs) const {
        return vec2<T>{x * rhs.x, y * rhs.y};
    }
    vec2<T> operator*(T const& rhs) const {
        return vec2<T>{x * rhs, y * rhs};
    }
};

template <typename T>
struct vec3 {
    union {
        struct {
            T x, y, z;
        };
        struct {
            T i, j, k;
        };
        T raw[3];
    };

    vec3<T> operator+(vec3<T> const& rhs) const {
        return vec3<T>{ x + rhs.x, y + rhs.y, z + rhs.z };
    }
    vec3<T> operator-(vec3<T> const& rhs) const {
        return vec3<T>{ x - rhs.x, y - rhs.y, z - rhs.z };
    }
    T operator*(vec3<T> const& rhs) const {
        return x * rhs.x + y * rhs.y + z * rhs.z;
    }
};

template <typename T>
struct vec4 {
    union {
        struct {
            T x, y, z, w;
        };
        T raw[4];
    };
};

namespace math {

template <typename T>
vec3<T> cross(vec3<T> const& a, vec3<T> const& b) {
    return vec3<T>{
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.y,
        a.x * b.y - a.y * b.x,
    };
}

template <typename T>
T magnitude(vec3<T> const& v) {
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

template <typename T>
vec3<T> normalise(vec3<T> const& v) {
    const auto m = magnitude(v);
    return vec3<T>{
        v.x / m,
        v.y / m,
        v.z / m
    };
}

};

class color {
   public:
    color() : color(0, 0, 0, 0) {}
    color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 0xFF)
        : m_red(red), m_green(green), m_blue(blue), m_alpha(alpha) {}
    color(uint32_t hex) {
        m_alpha = 0xFF;
        if (hex > 0xFFFFFF) {
            m_red   = (hex >> 24) & 0xFF;
            m_green = (hex >> 16) & 0xFF;
            m_blue  = (hex >>  8) & 0xFF;
            m_alpha = (hex >>  0) & 0xFF;
        } else {
            m_red   = (hex >> 16) & 0xFF;
            m_green = (hex >>  8) & 0xFF;
            m_blue  = (hex >>  0) & 0xFF;
        }
    }
    ~color() {}

   public:
    inline static color white() { return color(0xFFFFFF); }
    inline static color red()   { return color(0xFF0000); }
    inline static color green() { return color(0x00FF00); }
    inline static color blue()  { return color(0x0000FF); }

    uint8_t get_red()   const { return m_red;   }
    uint8_t get_green() const { return m_green; }
    uint8_t get_blue()  const { return m_blue;  }
    uint8_t get_alpha() const { return m_alpha; }

    void set_red(uint8_t value)   { m_red = value;   }
    void set_green(uint8_t value) { m_green = value; }
    void set_blue(uint8_t value)  { m_blue = value;  }
    void set_alpha(uint8_t value) { m_alpha = value; }

   public:
    std::string json() const {
        std::stringstream ss;
        ss << "{";
        ss << "\"_type\":\"tiny::color\",";
        ss << "\"red\":"   << (uint32_t)m_red   << ",";
        ss << "\"green\":" << (uint32_t)m_green << ",";
        ss << "\"blue\":"  << (uint32_t)m_blue  << ",";
        ss << "\"alpha\":" << (uint32_t)m_alpha;
        ss << "}";
        return ss.str();
    }
    friend std::ostream& operator<<(std::ostream& os, color const& c) {
        return os << c.json();
    }

   private:
    uint8_t m_red;
    uint8_t m_green;
    uint8_t m_blue;
    uint8_t m_alpha;
};

class image {
   public:
    image(int32_t width, int32_t height, int32_t channels = 3)
        : m_width(width), m_height(height), m_channels(channels) {
        m_buffer = new uint8_t[m_width * m_height * m_channels];
    }
    ~image() { delete[] m_buffer; }

   public:
    void set(int32_t const& x, int32_t const& y, color const& color) {
        if (!(x > -1 && x < m_width && y > -1 && y < m_height)) return;
        const auto index = (y * m_channels) * m_width + (x * m_channels);
        m_buffer[index + 0] = color.get_red();
        m_buffer[index + 1] = color.get_green();
        m_buffer[index + 2] = color.get_blue();
        if (m_channels == 4) m_buffer[index + 3] = color.get_alpha();
    }

    color get_color(int32_t const& x, int32_t const& y) {
        color c;
        if (!(x > -1 && x < m_width && y > -1 && y < m_height)) return c;
        const auto index = (y * m_channels) * m_width + (x * m_channels);
        c.set_red(m_buffer[index + 0]);
        c.set_green(m_buffer[index + 1]);
        c.set_blue(m_buffer[index + 2]);
        if (m_channels == 4) c.set_alpha(m_buffer[index + 3]);
        return c;
    }

    int32_t get_width()  const { return m_width;  }
    int32_t get_height() const { return m_height; }

   public:
    void write_png(std::string const& filename) {
        stbi_write_png(filename.c_str(), m_width, m_height, m_channels,
                       m_buffer, m_width * m_channels);
    }

    void flipv() {
        for (int32_t i = 0; i < m_height / 2; i++) {
            for (int32_t j = 0; j < m_width; j++) {
                const auto a = get_color(j, i);
                const auto b = get_color(j, m_height - 1 - i);
                set(j, i, b);
                set(j, m_height - 1 - i, a);
            }
        }
    }

    void fliph() {
        for (int32_t i = 0; i < m_height; i++) {
            for (int32_t j = 0; j < m_width / 2; j++) {
                const auto a = get_color(j, i);
                const auto b = get_color(m_width - 1 - j, i);
                set(j, i, b);
                set(m_width - 1 - j, i, a);
            }
        }
    }

   public:
    std::string json() const {
        std::stringstream ss;
        ss << "{";
        ss << "\"_type\":"
           << "\"tiny::image\",";
        ss << "\"width\":"    << m_width << ",";
        ss << "\"height\":"   << m_height << ",";
        ss << "\"channels\":" << m_channels;
        ss << "}";
        return ss.str();
    }
    friend std::ostream& operator<<(std::ostream& os, image const& image) {
        return os << image.json();
    }

   private:
    uint8_t* m_buffer;
    int32_t m_width;
    int32_t m_height;
    int32_t m_channels;
};

class model {
   public:
    model() : m_filename(""), m_loaded(false) {}
    model(std::string const& filename) : m_filename(filename), m_loaded(false) {
        load();
    }
    ~model() {}

    void load(std::string const& filename) {
        m_filename = filename;
        load();
    }

    bool is_load() const { return m_loaded; }

    int32_t nverts() const { return m_verts.size(); }
    int32_t nfaces() const { return m_faces.size(); }

    vec3<float> vert(int32_t i) { return m_verts[i]; }
    std::vector<int32_t> face(int32_t i) { return m_faces[i]; }

   private:
    void load() {
        if (m_loaded) return;
        std::ifstream file(m_filename, std::ios::in);
        if (file.fail() && !file.is_open()) return;

        std::string line;
        while (!file.eof()) {
            std::getline(file, line);
            std::istringstream iss(line.c_str());
            char trash;
            if (!line.compare(0, 2, "v ")) {
                iss >> trash;
                vec3<float> vertex;
                iss >> vertex.x;
                iss >> vertex.y;
                iss >> vertex.z;
                m_verts.push_back(vertex);
            } else if (!line.compare(0, 2, "f ")) {
                std::vector<int32_t> face;
                int32_t itrash, idx;
                iss >> trash;
                while (iss >> idx >> trash >> itrash >> trash >> itrash) {
                    face.push_back(--idx);
                }
                m_faces.push_back(face);
            }
        }

        file.close();
        m_loaded = true;
    }

   public:
    friend std::ostream& operator<<(std::ostream& os, model const& model) {
        os << "tiny::model {";
        os << " loaded: " << (model.m_loaded ? "true" : "false") << ",";
        os << " filename: " << model.m_filename << ",";
        os << " vertices: " << model.m_verts.size() << ",";
        os << " faces: " << model.m_faces.size() << " }";
        return os;
    }

   private:
    std::string m_filename;
    bool m_loaded;

    std::vector<vec3<float>> m_verts;
    std::vector<std::vector<int32_t>> m_faces;
};

}  // namespace tiny

