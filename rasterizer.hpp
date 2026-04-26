#pragma once
#include "math.hpp"
#include <vector>

struct Color
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

struct Barycentric
{
    float alpha;
    float beta;
    float gamma;
};

struct Vertex
{
    Vec3 position;
    Color color;
};

// p to test
// a->p (p.x - a.x, p.y -a. y, 0)
// a->b (b.x - a.x, b.y - a.y, 0)
// cross(a->p, a->b)
// cross product (0, 0, (b.y-a.y)*(p.x-a.x)-(b.x-a.x)*(p.y-a.y))
float edge_function(Vec2 a, Vec2 b, Vec2 p);

// a->b是否属于top-left边
bool is_top_left_edge(Vec2 a, Vec2 b);

bool edge_accepts_pixel(float edge_value, Vec2 a, Vec2 b);

// a, b, c 三角形的三个顶点
// p 当前像素中心点
bool inside_triangle(Vec2 a, Vec2 b, Vec2 c, Vec2 p);

Barycentric barycentric(Vec2 a, Vec2 b, Vec2 c, Vec2 p);

void draw_triangle(std::vector<unsigned char> &framebuffer, size_t width, size_t height, Vertex a, Vertex b, Vertex c);