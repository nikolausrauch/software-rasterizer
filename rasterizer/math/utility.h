#pragma once

#include "vector2.h"
#include "vector3.h"
#include "vector4.h"

Vec2 equirectangularUV(const Vec3& dir);
Vec3 equirectangulatDir(Vec2 pixel, Vec2i size);


template<typename T>
Vec3 barycentric(const Vector2<T>& a, const Vector2<T>& b, const Vector2<T>& c, const Vector2<T>& p)
{
    auto v_0 = b - a;
    auto v_1 = c - a;
    auto v_2 = p - a;

    float D = v_0.x * v_1.y - v_1.x * v_0.y;

    float v = (v_2.x * v_1.y - v_1.x * v_2.y) / D;
    float w = (v_0.x * v_2.y - v_2.x * v_0.y) / D;
    float u = 1.0f - v - w;

    return {u, v, w};
}

template<typename T>
Vec3 barycentric(const Vector3<T>& a, const Vector3<T>& b, const Vector3<T>& c, const Vector3<T>& p)
{
    auto v_0 = b - a;
    auto v_1 = c - a;
    auto v_2 = p - a;

    float d_00 = dot(v_0, v_0);
    float d_01 = dot(v_0, v_1);
    float d_11 = dot(v_1, v_1);
    float d_20 = dot(v_2, v_0);
    float d_21 = dot(v_2, v_1);

    float D = d_00 * d_11 - d_01 * d_01;

    float v = (d_11 * d_20 - d_01 * d_21) / D;
    float w = (d_00 * d_21 - d_01 * d_20) / D;
    float u = 1.0f - v - w;

    return {u, v, w};
}
