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

Vec3 corners[8] = {
    {-0.5f, -0.5f, -0.5f}, // 0
    {+0.5f, -0.5f, -0.5f}, // 1
    {+0.5f, +0.5f, -0.5f}, // 2
    {-0.5f, +0.5f, -0.5f}, // 3
    {-0.5f, -0.5f, +0.5f}, // 4
    {+0.5f, -0.5f, +0.5f}, // 5
    {+0.5f, +0.5f, +0.5f}, // 6
    {-0.5f, +0.5f, +0.5f}, // 7
};

// 36 个索引：6 个面 × 2 个三角形 × 3 个顶点
int indices[36] = {
    0, 1, 2, 0, 2, 3,
    4, 5, 6, 4, 6, 7,
    5, 1, 2, 5, 2, 6,
    4, 0, 3, 4, 3, 7,
    7, 6, 2, 7, 2, 3,
    4, 5, 1, 4, 1, 0};

Color face_color[6] = {{255, 0, 0}, {0, 255, 0}, {0, 0, 255}, {255, 255, 0}, {255, 0, 255}, {0, 255, 255}};

int main()
{
    Vertex cube[36];
    for (int i = 0; i < 36; i++)
    {
        cube[i].color = face_color[i / 6];
        cube[i].position = corners[indices[i]];
    }
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

    auto start = std::chrono::steady_clock::now();
    Mat4 model = Mat4::rotate_z(PI / 6);
    // model = Mat4::identity();
    Mat4 view = Mat4::look_at({3, 0, 0}, {0, 0, 0}, {0, 1, 0});
    Mat4 perspective = Mat4::perspective(PI / 3, float(WIDTH)/float(HEIGHT), 0.5f, 5.0f);
    for (int i = 0; i < 36; i++)
    {
        Vec4 clip = perspective * view * model * Vec4::from_point(cube[i].position);
        cube[i].position = {clip.x / clip.w, clip.y / clip.w, clip.z / clip.w};
    }
    for (int i = 0; i < 36; i += 3)
    {
        cube[i].position = ndc_to_screen(cube[i].position, WIDTH, HEIGHT);
        cube[i + 1].position = ndc_to_screen(cube[i + 1].position, WIDTH, HEIGHT);
        cube[i + 2].position = ndc_to_screen(cube[i + 2].position, WIDTH, HEIGHT);
        draw_triangle(framebuffer, WIDTH, HEIGHT, cube[i], cube[i + 1], cube[i + 2]);
    }

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