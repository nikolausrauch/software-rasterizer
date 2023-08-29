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

    Vector3()
        : x(0), y(0), z(0)
    {

    }

    Vector3(T v)
        : x(v), y(v), z(v)
    {

    }

    Vector3(T x, T y, T z)
        : x(x), y(y), z(z)
    {

    }

    template<typename U>
    Vector3(const Vector2<U>& a, T v)
        : x(a.x), y(a.y), z(v)
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

    Vector3& operator /=(Vector3 s)
    {
        assert(s.x != 0 && s.y != 0 && s.z != 0);
        x /= s.x;
        y /= s.y;
        z /= s.z;

        return *this;
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

    Vector3& operator +=(const T v)
    {
        x += v;
        y += v;
        z += v;

        return *this;
    }

    Vector3& operator -=(const T v)
    {
        x -= v;
        y -= v;
        z -= v;

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

template<typename T>
auto operator +(const Vector3<T>& a, const T v)
{
    return Vector3<T>(a.x + v, a.y + v, a.z + v);
}

template<typename T>
auto operator -(const Vector3<T>& a, const T v)
{
    return Vector3<T>(a.x - v, a.y - v, a.z - v);
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
auto pow(const Vector3<T>& a, const Vector3<U>& b)
{
    return Vector3<decltype(T() * U())>{ std::pow(a.x, b.x), std::pow(a.y, b.y), std::pow(a.z, b.z) };
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

template<typename T, typename U>
auto reflect(const Vector3<T>& I, const Vector3<U>& N)
{
    return I - 2.0f * dot(N, I) * N;
}

template<typename T, typename U>
auto project(const Vector3<T>& a, const Vector3<U>& b)
{
    return (b * (dot(a, b) / dot(b, b)));
}


template<typename T>
auto mix(const Vector3<T>& a, const Vector3<T>& b, float t)
{
    t = std::clamp(t, 0.0f, 1.0f);
    return (1.0f - t) * a + t * b;
}

template<typename T>
Vector3<T> min(const Vector3<T>& a, const Vector3<T>& b)
{
    return
    {
        std::min(a.x, b.x),
        std::min(a.y, b.y),
        std::min(a.z, b.z)
    };
}


template<typename T>
Vector3<T> max(const Vector3<T>& a, const Vector3<T>& b)
{
    return
    {
        std::max(a.x, b.x),
        std::max(a.y, b.y),
        std::max(a.z, b.z)
    };
}

typedef Vector3<int> Vec3i;
typedef Vector3<float> Vec3;
typedef Vector3<std::uint8_t> Vec3u_8;

