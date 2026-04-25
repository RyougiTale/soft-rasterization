#include <iostream>
#include <fstream>
#include <vector>
#include <cstddef>
#include <algorithm>
#include <cmath>
#include <cassert>
#include <utility>
#include <chrono>

struct Color
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

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

struct Vec3
{
    float x;
    float y;
    float z;
};

struct Barycentric
{
    float alpha;
    float beta;
    float gamma;
};

struct Vertex
{
    Vec2 position;
    Color color;
};

// p to test
// a->p (p.x - a.x, p.y -a. y, 0)
// a->b (b.x - a.x, b.y - a.y, 0)
// cross(a->p, a->b)
// cross product (0, 0, (b.y-a.y)*(p.x-a.x)-(b.x-a.x)*(p.y-a.y))
float edge_function(Vec2 a, Vec2 b, Vec2 p)
{
    return cross_z(p - a, b - a);
}

// a->b是否属于top-left边
bool is_top_left_edge(Vec2 a, Vec2 b)
{
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    return dy < 0 || (dy == 0 && dx > 0);
}

bool edge_accepts_pixel(float edge_value, Vec2 a, Vec2 b)
{
    if (edge_value > 0)
        return true;
    else if (edge_value == 0 && is_top_left_edge(a, b))
        return true;
    return false;
}

// a, b, c 三角形的三个顶点
// p 当前像素中心点
bool inside_triangle(Vec2 a, Vec2 b, Vec2 c, Vec2 p)
{
    // Top-left fill rule for shared edges
    float w0 = edge_function(a, b, p);
    if (!edge_accepts_pixel(w0, a, b))
        return false;
    float w1 = edge_function(b, c, p);
    if (!edge_accepts_pixel(w1, b, c))
        return false;
    float w2 = edge_function(c, a, p);
    if (!edge_accepts_pixel(w2, c, a))
        return false;
    return true;
}

Barycentric barycentric(Vec2 a, Vec2 b, Vec2 c, Vec2 p)
{
    float sum = edge_function(a, b, c);
    assert(sum != 0);
    float alpha = edge_function(p, b, c) / sum;
    float beta = edge_function(a, p, c) / sum;
    float gamma = edge_function(a, b, p) / sum;
    return {alpha, beta, gamma};
}

// deprecated: use top-left coverage now
// bool inside_barycentric(Barycentric weights)
// {
//     return weights.alpha >= 0 && weights.beta >= 0 && weights.gamma >= 0;
// }

void draw_triangle(std::vector<unsigned char> &framebuffer, size_t width, size_t height, Vertex a, Vertex b, Vertex c)
{
    // clockwise
    float area = edge_function(a.position, b.position, c.position);
    if (area == 0) // 将来需要epsilon或fix-points
    {
        std::cerr << "area == 0\n";
        return;
    }
    else if (area < 0)
    {
        std::swap(b, c);
    }
    float min_x = std::min({a.position.x, b.position.x, c.position.x});
    float max_x = std::max({a.position.x, b.position.x, c.position.x});
    float min_y = std::min({a.position.y, b.position.y, c.position.y});
    float max_y = std::max({a.position.y, b.position.y, c.position.y});

    min_x = std::clamp(min_x, 0.0f, static_cast<float>(width));
    max_x = std::clamp(max_x, 0.0f, static_cast<float>(width));
    min_y = std::clamp(min_y, 0.0f, static_cast<float>(height));
    max_y = std::clamp(max_y, 0.0f, static_cast<float>(height));

    for (size_t y = std::floor(min_y); y < std::ceil(max_y); y++)
    {
        for (size_t x = std::floor(min_x); x < std::ceil(max_x); x++)
        {
            Vec2 p = {x + 0.5f, y + 0.5f};
            if (inside_triangle(a.position, b.position, c.position, p))
            {
                auto [alpha, beta, gamma] = barycentric(a.position, b.position, c.position, p);
                framebuffer[3 * (width * y + x)] = alpha * a.color.r + beta * b.color.r + gamma * c.color.r;
                framebuffer[3 * (width * y + x) + 1] = alpha * a.color.g + beta * b.color.g + gamma * c.color.g;
                framebuffer[3 * (width * y + x) + 2] = alpha * a.color.b + beta * b.color.b + gamma * c.color.b;
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

    Vertex a{{100.0f, 100.0f}, {255, 0, 0}};
    Vertex b{{700.0f, 150.0f}, {0, 255, 0}};
    Vertex c{{300.0f, 300.0f}, {0, 0, 255}};
    auto start = std::chrono::steady_clock::now();
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