#pragma once

#include <cstddef>

constexpr float PI = 3.14159265358979323846f;

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
    union
    {
        struct
        {
            float x;
            float y;
            float z;
        };
        float m[3];
    };
    inline Vec2 xy() { return {x, y}; }
    float &operator[](size_t i) { return m[i]; }
    const float &operator[](size_t i) const { return m[i]; }
};

struct Vec4
{
    union
    {
        struct
        {
            float x;
            float y;
            float z;
            float w;
        };
        float m[4];
    };
    inline Vec3 xyz() { return {{{x, y, z}}}; }
    float &operator[](size_t i) { return m[i]; }
    const float &operator[](size_t i) const { return m[i]; }
    static Vec4 from_point(Vec3 v);  // w = 1
    static Vec4 from_vector(Vec3 v); // w = 0
};

struct Mat4
{
    float m[4][4];
    static Mat4 identity();
    static Mat4 translation(Vec3 t);
    static Mat4 rotate_x(float theta);
    static Mat4 rotate_y(float theta);
    static Mat4 rotate_z(float theta);
    float *operator[](size_t i)
    {
        return m[i];
    }

    const float *operator[](size_t i) const
    {
        return m[i];
    }
    Mat4 operator*(const Mat4 &rhs) const
    {
        Mat4 ret;
        // ret[0][0] = m[0][0] * rhs[0][0] + m[0][1] * rhs[1][0] + m[0][2] * rhs[2][0] + m[0][3] * rhs[3][0];

        for (size_t i = 0; i < 4; i++)
        {
            for (size_t j = 0; j < 4; j++)
            {
                ret[i][j] = m[i][0] * rhs[0][j] + m[i][1] * rhs[1][j] + m[i][2] * rhs[2][j] + m[i][3] * rhs[3][j];
            }
        }
        return ret;
    }
    Vec4 operator*(const Vec4 &rhs) const
    {
        Vec4 ret;
        for (size_t i = 0; i < 4; i++)
        {
            ret[i] = m[i][0] * rhs[0] + m[i][1] * rhs[1] + m[i][2] * rhs[2] + m[i][3] * rhs[3];
        }
        return ret;
    }
};