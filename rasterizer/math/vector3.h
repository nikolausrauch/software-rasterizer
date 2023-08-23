#pragma once

#include "base.h"

#include "vector2.h"

#include <iostream>
#include <iomanip>

template<typename T> struct Vector4;

template<typename T>
struct Vector3
{
    T x, y, z;


    Vector3(T x = 0, T y = 0, T z = 0)
        : x(x), y(y), z(z)
    {

    }

    template<typename U>
    Vector3(const Vector3<U>& a)
        : x(a.x), y(a.y), z(a.z)
    {

    }

    template<typename U>
    Vector3(const Vector4<U>& a)
        : x(a.x), y(a.y), z(a.z)
    {

    }

    Vector3 operator -() const
    {
        return Vector3(-x, -y, -z);
    }

    Vector3& operator *=(T s)
    {
        x *= s;
        y *= s;
        z *= s;

        return *this;
    }

    Vector3& operator /=(T s)
    {
        assert(s != 0.0f);
        return *this *= (1.0 / s);
    }

    Vector3& operator +=(const Vector3& v)
    {
        x += v.x;
        y += v.y;
        z += v.z;

        return *this;
    }

    Vector3& operator -=(const Vector3& v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;

        return *this;
    }

    float& operator [](unsigned int i)
    {
        assert(i < 3);
        return (&x)[i];
    }

    const float& operator [](unsigned int i) const
    {
        assert(i < 3);
        return (&x)[i];
    }

    friend std::ostream& operator<<(std::ostream& os, const Vector3<T>& vec)
    {
        return os << "{" << +vec.x << ", " << +vec.y << ", " << +vec.z << "}";
    }

    using AccumType = Vector3<float>;
};

template<typename T, typename U>
auto operator *(const Vector3<T>& a, const Vector3<U>& b)
{
    return Vector3<decltype(T() * U())>(a.x * b.x, a.y * b.y, a.z * b.z);
}

template<typename T, typename U>
auto operator /(const Vector3<T>& a, const Vector3<U>& b)
{
    return Vector3<decltype(T() / U())>(a.x / b.x, a.y / b.y, a.z / b.z);
}

template<typename T, typename U>
auto operator +(const Vector3<T>& a, const Vector3<U>& b)
{
    return Vector3<decltype(T() + U())>(a.x + b.x, a.y + b.y, a.z + b.z);
}

template<typename T, typename U>
auto operator -(const Vector3<T>& a, const Vector3<U>& b)
{
    return Vector3<decltype(T() - U())>(a.x - b.x, a.y - b.y, a.z - b.z);
}

template<typename T, typename U>
auto operator *(const Vector3<T>& v, U s)
{
    return Vector3<decltype(T() * U())>(v.x * s, v.y * s, v.z * s);
}

template<typename T, typename U>
auto operator /(const Vector3<T>& v, U s)
{
    return Vector3<decltype(T() / U())>(v.x / s, v.y / s, v.z / s);
}

template<typename T, typename U>
auto operator *(U s, const Vector3<T>& v)
{
    return Vector3<decltype(T() * U())>(v.x * s, v.y * s, v.z * s);
}

template<typename T, typename U>
auto operator /(U s, const Vector3<T>& v)
{
    return Vector3<decltype(T() / U())>(s / v.x, s / v.y, s / v.z);
}

template<typename T>
auto operator ==(const Vector3<T>& a, const Vector3<T>& b)
{
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

template<typename T>
auto operator !=(const Vector3<T>& a, const Vector3<T>& b)
{
    return a.x != b.x && a.y != b.y && a.z != b.z;
}

template<typename T>
float length(const Vector3<T>& v)
{
    return std::sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

template<typename T>
Vector3<float> normalize(const Vector3<T>& v)
{
    assert(length(v) != 0.0f);
    return v / length(v);
}

template<typename T>
auto abs(const Vector3<T>& v)
{
    return Vector3<T>{ std::abs(v.x), std::abs(v.y), std::abs(v.z) };
}

template<typename T>
auto clamp(const Vector3<T>& v, const Vector3<T>& min, const Vector3<T>& max)
{
    return Vector3<T>{ std::clamp<T>(v.x, min.x, max.x), std::clamp<T>(v.y, min.y, max.y), std::clamp<T>(v.z, min.z, max.z)  };
}

template<typename T, typename U>
auto dot(const Vector3<T>& a, const Vector3<U>& b)
{
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

template<typename T, typename U>
auto cross(const Vector3<T>& a, const Vector3<U>& b)
{
    return Vector3<decltype(T() * U())>(
                a.y * b.z - a.z * b.y,
                a.z * b.x - a.x * b.z,
                a.x * b.y - a.y * b.x
                );
}

template<typename T>
Vector3<float> barycentric(const Vector2<T>& a, const Vector2<T>& b, const Vector2<T>& c, const Vector2<T>& p)
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
Vector3<float> barycentric(const Vector3<T>& a, const Vector3<T>& b, const Vector3<T>& c, const Vector3<T>& p)
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

template<typename T, typename U>
auto reflect(const Vector3<T>& I, const Vector3<U>& N)
{
    return I - 2.0f * dot(I, N) * N;
}

template<typename T, typename U>
auto project(const Vector3<T>& a, const Vector3<U>& b)
{
    return (b * (dot(a, b) / dot(b, b)));
}

typedef Vector3<int> Vec3i;
typedef Vector3<float> Vec3;
typedef Vector3<std::uint8_t> Vec3u_8;


//Vector3 project(const Vector3 &a, const Vector3 &b)
//{
//    return (b * (dot(a, b) / dot(b, b)));
//}

//Vector3 reject(const Vector3 &a, const Vector3 &b)
//{
//    return (a - b * (dot(a, b) / dot(b, b)));
//}


