#pragma once

struct Vec2
{
    float x;
    float y;
    Vec2 operator-(Vec2 rhs) const
    {
        return Vec2{x - rhs.x, y - rhs.y};
    }
};

inline float cross_z(Vec2 lhs, Vec2 rhs)
{
    return lhs.x * rhs.y - lhs.y * rhs.x;
}

struct Vec3
{
    float x;
    float y;
    float z;
};