#include "rasterizer.hpp"
#include <iostream>
#include <assert.h>
#include <algorithm>
#include <cmath>
#include <utility>

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