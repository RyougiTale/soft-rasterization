#include <iostream>
#include <fstream>
#include <vector>
#include <cstddef>
#include <algorithm>
#include <cmath>

struct Vec2
{
    float x;
    float y;
    Vec2 operator-(Vec2 rhs) const
    {
        return Vec2{x - rhs.x, y - rhs.y};
    }
};
float cross_z(Vec2 lhs, Vec2 rhs)
{
    return lhs.x * rhs.y - lhs.y * rhs.x;
}

// p to test
// a->p (p.x - a.x, p.y -a. y, 0)
// a->b (b.x - a.x, b.y - a.y, 0)
// cross(a->p, a->b)
// cross product (0, 0, (b.y-a.y)*(p.x-a.x)-(b.x-a.x)*(p.y-a.y))
float edge_function(Vec2 a, Vec2 b, Vec2 p)
{
    return cross_z(p - a, b - a);
}

// a, b, c 三角形的三个顶点
// p 当前像素中心点
bool inside_triangle(Vec2 a, Vec2 b, Vec2 c, Vec2 p)
{
    float w0 = edge_function(a, b, p);
    float w1 = edge_function(b, c, p);
    float w2 = edge_function(c, a, p);
    // todo: 共边的情况
    // Top-left fill rule for shared edges
    if (w0 >= 0 && w1 >= 0 && w2 >= 0)
        return true;
    if (w0 <= 0 && w1 <= 0 && w2 <= 0)
        return true;
    return false;
}

void draw_triangle(std::vector<unsigned char> &framebuffer, size_t width, size_t height, Vec2 a, Vec2 b, Vec2 c, unsigned char r_color, unsigned char g_color, unsigned char b_color)
{
    float min_x = std::min({a.x, b.x, c.x});
    float max_x = std::max({a.x, b.x, c.x});
    float min_y = std::min({a.y, b.y, c.y});
    float max_y = std::max({a.y, b.y, c.y});

    min_x = std::clamp(min_x, 0.0f, static_cast<float>(width));
    max_x = std::clamp(max_x, 0.0f, static_cast<float>(width));
    min_y = std::clamp(min_y, 0.0f, static_cast<float>(height));
    max_y = std::clamp(max_y, 0.0f, static_cast<float>(height));

    for (size_t y = std::floor(min_y); y < std::ceil(max_y); y++)
    {
        for (size_t x = std::floor(min_x); x < std::ceil(max_x); x++)
        {
            Vec2 p = {x + 0.5f, y + 0.5f};
            if (inside_triangle(a, b, c, p))
            {
                framebuffer[3 * (width * y + x)] = r_color;
                framebuffer[3 * (width * y + x) + 1] = g_color;
                framebuffer[3 * (width * y + x) + 2] = b_color;
            }
        }
    }
}

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

    Vec2 a{100.0f, 100.0f};
    Vec2 b{700.0f, 150.0f};
    Vec2 c{300.0f, 300.0f};
    draw_triangle(framebuffer, WIDTH, HEIGHT, a, b, c, 255, 0, 0);

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