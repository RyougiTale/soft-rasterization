#include "math.hpp"
#include <cmath>

Vec4 Vec4::from_point(Vec3 v) // w = 1
{
    return {v.x, v.y, v.z, 1.0f};
}

Vec4 Vec4::from_vector(Vec3 v) // w = 0
{
    return {v.x, v.y, v.z, 0.0f};
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

Mat4 Mat4::look_at(Vec3 eye, Vec3 target, Vec3 up)
{
    Vec3 forward = (target - eye).normalize();
    Vec3 right = forward.cross(up).normalize();
    Vec3 true_up = right.cross(forward);
    Mat4 rotate = Mat4{{{right.x, right.y, right.z, 0},
                        {true_up.x, true_up.y, true_up.z, 0},
                        {-forward.x, -forward.y, -forward.z, 0},
                        {0, 0, 0, 1}}};
    // {-dot(right, eye), -dot(true_up, eye), dot(forward, eye), 1}
    return rotate * translation(-eye);
}

Mat4 Mat4::perspective(float fov_y, float aspect, float near, float far)
{
    float f = 1.0f / std::tan(fov_y / 2);
    return Mat4{{{f / aspect, 0, 0, 0},
                 {0, f, 0, 0},
                 {0, 0, (near + far) / (near - far), 2 * near * far / (near - far)},
                 {0, 0, -1, 0}}};
}