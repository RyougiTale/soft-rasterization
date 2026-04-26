#include <iostream>
#include <fstream>
#include <vector>
#include <cstddef>
#include <chrono>
#include "math.hpp"
#include "rasterizer.hpp"

// pixels: contiguous RGB arrays
bool write_ppm(const char *filename, size_t width, size_t height, const std::vector<unsigned char> &pixels)
{
    if (pixels.size() != width * height * 3)
    {
        std::cerr << "file size not equal\n";
        return false;
    }
    auto f = std::ofstream(filename, std::ios::binary);
    if (f.is_open())
    {

        f << "P6\n";
        f << width << ' ' << height << '\n';
        f << 255 << '\n';
        f.write(reinterpret_cast<const char *>(pixels.data()), width * height * 3);
        if (!f)
        {
            std::cerr << "fail to write ppm data\n";
            return false;
        }
    }
    else
    {
        std::cerr << "file open fail\n";
        return false;
    }
    return true;
}

void clear_framebuffer(std::vector<unsigned char> &framebuffer, size_t width, size_t height, unsigned char r, unsigned char g, unsigned char b)
{
    for (size_t i = 0; i < width * height * 3; i += 3)
    {
        framebuffer[i] = r;
        framebuffer[i + 1] = g;
        framebuffer[i + 2] = b;
    }
}

int main()
{
    constexpr const char *FILENAME = "pic.ppm";
    // 800 * 600 * 3 = 1,440,000 bytes = 1.37MB
    constexpr size_t WIDTH = 800;
    constexpr size_t HEIGHT = 600;
    std::vector<unsigned char> framebuffer(WIDTH * HEIGHT * 3);
    clear_framebuffer(framebuffer,
                      WIDTH,
                      HEIGHT,
                      static_cast<unsigned char>(0),
                      static_cast<unsigned char>(0),
                      static_cast<unsigned char>(0));

    Vertex a{{{{100.0f, 100.0f, 0.0f}}}, {255, 0, 0}};
    Vertex b{{{{700.0f, 150.0f, 0.0f}}}, {0, 255, 0}};
    Vertex c{{{{300.0f, 300.0f, 0.0f}}}, {0, 0, 255}};
    auto start = std::chrono::steady_clock::now();
    Mat4 model = Mat4::rotate_z(PI / 6);
    auto a_model = model * Vec4::from_point(a.position);
    a.position = Vec3{{{a_model.x, a_model.y, a_model.z}}};
    auto b_model = model * Vec4::from_point(b.position);
    b.position = Vec3{{{b_model.x, b_model.y, b_model.z}}};
    auto c_model = model * Vec4::from_point(c.position);
    c.position = Vec3{{{c_model.x, c_model.y, c_model.z}}};

    draw_triangle(framebuffer, WIDTH, HEIGHT, a, b, c);
    auto end = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << elapsed.count() << " us\n";

    if (write_ppm(FILENAME, WIDTH, HEIGHT, framebuffer))
    {
        std::cout << "generate soft-rasterization success\n";
        return 0;
    }
    else
    {
        std::cerr << "generate soft-rasterization fail\n";
        return -1;
    }
}