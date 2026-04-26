#include "math.hpp"
#include <cmath>

Vec4 Vec4::from_point(Vec3 v) // w = 1
{
    return {{{v.x, v.y, v.z, 1.0f}}};
}

Vec4 Vec4::from_vector(Vec3 v) // w = 0
{
    return {{{v.x, v.y, v.z, 0.0f}}};
}

Mat4 Mat4::identity()
{
    Mat4 mat = {{{1.0f, 0.0f, 0.0f, 0.0f},
                 {0.0f, 1.0f, 0.0f, 0.0f},
                 {0.0f, 0.0f, 1.0f, 0.0f},
                 {0.0f, 0.0f, 0.0f, 1.0f}}};

    return mat;
}

Mat4 Mat4::translation(Vec3 v)
{
    Mat4 mat = {{{1.0f, 0.0f, 0.0f, v.x},
                 {0.0f, 1.0f, 0.0f, v.y},
                 {0.0f, 0.0f, 1.0f, v.z},
                 {0.0f, 0.0f, 0.0f, 1.0f}}};

    return mat;
}

Mat4 Mat4::rotate_x(float theta)
{
    Mat4 mat = {{{1.0f, 0.0f, 0.0f, 0.0f},
                 {0.0f, std::cos(theta), -std::sin(theta), 0.0f},
                 {0.0f, std::sin(theta), std::cos(theta), 0.0f},
                 {0.0f, 0.0f, 0.0f, 1.0f}}};
    return mat;
}
Mat4 Mat4::rotate_y(float theta)
{
    Mat4 mat = {{{std::cos(theta), 0.0f, -std::sin(theta), 0.0f},
                 {0.0f, 1.0f, 0.0f, 0.0f},
                 {std::sin(theta), 0.0f, std::cos(theta), 0.0f},
                 {0.0f, 0.0f, 0.0f, 1.0f}}};
    return mat;
}
Mat4 Mat4::rotate_z(float theta)
{
    Mat4 mat = {{{std::cos(theta), -std::sin(theta), 0.0f, 0.0f},
                 {std::sin(theta), std::cos(theta), 0.0f, 0.0f},
                 {0.0f, 0.0f, 1.0f, 0.0f},
                 {0.0f, 0.0f, 0.0f, 1.0f}}};
    return mat;
}