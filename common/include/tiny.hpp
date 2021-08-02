#pragma once

#include <cstdint>
#include <string>
#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <sstream>

#include "stb/stb_image_write.h"

namespace tiny {

    // TODO: Write operator overload for the vec data types

    template <typename T>
    struct vec2 {
        union {
            struct { T x, y; };
            struct { T u, v; };
            T raw[2];
        };
    };

    template <typename T>
    struct vec3 {
        union {
            struct { T x, y, z; };
            T raw[3];
        };
    };

    template <typename T>
    struct vec4 {
        union {
            struct { T x, y, z, w; };
            T raw[4];
        };
    };

    class color {
       public:
        color() : color(0, 0, 0, 0) {}
        color(uint8_t red, uint8_t green,
              uint8_t blue, uint8_t alpha = 0xFF)
            : m_red(red), m_green(green), m_blue(blue), m_alpha(alpha) {}
        color(uint32_t hex) {
            if (hex > 0xFFFFFF) {
                m_red   = (hex >> 24) & 0xFF;
                m_green = (hex >> 16) & 0xFF;
                m_blue  = (hex >>  8) & 0xFF;
                m_alpha = (hex >>  0) & 0xFF;
            } else {
                m_red   = (hex >> 16) & 0xFF;
                m_green = (hex >>  8) & 0xFF;
                m_blue  = (hex >>  0) & 0xFF;
                m_alpha = 0xFF;
            }
        }
        ~color() {}

       public:
        uint8_t red()   const { return m_red;   }
        uint8_t green() const { return m_green; }
        uint8_t blue()  const { return m_blue;  }
        uint8_t alpha() const { return m_alpha; }

        void set_red(uint8_t value)   { m_red   = value; }
        void set_green(uint8_t value) { m_green = value; }
        void set_blue(uint8_t value)  { m_blue  = value; }
        void set_alpha(uint8_t value) { m_alpha = value; }

        friend std::ostream &operator<<(std::ostream &os, color const& color) {
            os << "tiny::color { ";
            os << std::to_string(color.m_red) << ", ";
            os << std::to_string(color.m_green) << ", ";
            os << std::to_string(color.m_blue) << ", ";
            os << std::to_string(color.m_alpha) << " }";
            return os;
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
          ~image() {
              delete[] m_buffer;
          }

        public:
          void set(int32_t const& x, int32_t const& y, color const& color) {
              if (!(x > -1 && x < m_width && y > -1 && y < m_height)) return;
              const auto index = (y * m_channels) * m_width + (x * m_channels);
              m_buffer[index + 0] = color.red();
              m_buffer[index + 1] = color.green();
              m_buffer[index + 2] = color.blue();
              if (m_channels == 4)
                  m_buffer[index + 3] = color.alpha();
          }

          color get_color(int32_t const& x, int32_t const& y) {
              color c;
              if (!(x > -1 && x < m_width && y > -1 && y < m_height)) return c;
              const auto index = (y * m_channels) * m_width + (x * m_channels);
              c.set_red(m_buffer[index + 0]);
              c.set_green(m_buffer[index + 1]);
              c.set_blue(m_buffer[index + 2]);
              if (m_channels == 4)
                  c.set_alpha(m_buffer[index + 3]);
              return c;
          }

          void write_png(std::string const& filename) {
              stbi_write_png(filename.c_str(), m_width, m_height,
                      m_channels, m_buffer, m_width * m_channels);
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
          friend std::ostream &operator<<(std::ostream &os, image const& image) {
              os << "tiny::image {";
              os << " width: "    << image.m_width    << ",";
              os << " height: "   << image.m_height   << ",";
              os << " channels: " << image.m_channels << " }";
              return os;
          }

        private:
          uint8_t *m_buffer;
          int32_t m_width;
          int32_t m_height;
          int32_t m_channels;
    };

    class model {
        public:
          model() : m_filename(""), m_loaded(false) {}
          model(std::string const& filename)
              : m_filename(filename), m_loaded(false) { load(); }
          ~model() {}

          void load(std::string const& filename) {
              m_filename = filename;
              load();
          }

          bool is_load() const { return m_loaded; }

          int32_t nverts() const { return m_verts.size(); }
          int32_t nfaces() const { return m_faces.size(); }

          vec3<float> vert(int32_t i) { return m_verts[i];}
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
          friend std::ostream &operator<<(std::ostream &os, model const& model) {
              os << "tiny::model {";
              os << " loaded: "   << (model.m_loaded ? "true" : "false") << ",";
              os << " filename: " << model.m_filename     << ",";
              os << " vertices: " << model.m_verts.size() << ",";
              os << " faces: "    << model.m_faces.size() << " }";
              return os;
          }

        private:
          std::string m_filename;
          bool        m_loaded;

          std::vector<vec3<float>>          m_verts;
          std::vector<std::vector<int32_t>> m_faces;
    };

}  // namespace tiny

