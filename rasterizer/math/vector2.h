#pragma once

#include "base.h"

#include <algorithm>
#include <cassert>
#include <iostream>

template<typename T> struct Vector4;
template<typename T> struct Vector3;

template<typename T>
struct Vector2
{
    T x, y;

    Vector2()
        : x(0), y(0)
    {

    }

    Vector2(T v)
        : x(v), y(v)
    {

    }

    Vector2(T x, T y)
        : x(x), y(y)
    {

    }

    Vector2(const Vector4<T>& a)
        : x(a.x), y(a.y)
    {

    }

    Vector2(const Vector3<T>& a)
        : x(a.x), y(a.y)
    {

    }

    template<typename U>
    Vector2(const Vector2<U>& a)
        : x(a.x), y(a.y)
    {

    }

    Vector2 operator-() const
    {
        return Vector2(-x, -y);
    }


    Vector2& operator*=(T s)
    {
        x *= s;
        y *= s;
        return *this;
    }

    Vector2& operator*=(Vector2 a)
    {
        x *= a.x;
        y *= a.y;
        return *this;
    }

    Vector2& operator/=(T s)
    {
        assert(s != 0.0f);
        return *this *= (1.0 / s);
    }

    Vector2& operator/=(Vector2 s)
    {
        assert(s.x != 0 && s.y != 0);
        x /= s.x;
        y /= s.y;
        return *this;
    }

    Vector2& operator+=(const Vector2 &v)
    {
        x += v.x;
        y += v.y;
        return *this;
    }

    Vector2& operator+=(const T v)
    {
        x += v;
        y += v;
        return *this;
    }

    Vector2& operator-=(const Vector2 &v)
    {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    Vector2& operator-=(const T v)
    {
        x -= v;
        y -= v;
        return *this;
    }

    T& operator[](unsigned int i)
    {
        assert(i < 2);
        return (&x)[i];
    }

    const T& operator[](unsigned int i) const
    {
        assert(i < 2);
        return (&x)[i];
    }

    friend std::ostream& operator<<(std::ostream& os, const Vector2<T>& vec)
    {
        return os << "{" << +vec.x << ", " << +vec.y <<  "}";
    }
};


template<typename T, typename U>
auto operator *(const Vector2<T>& a, const Vector2<U>& b)
{
    return Vector2<decltype(T() * T())>(a.x * b.x, a.y * b.y);
}

template<typename T, typename U>
auto operator +(const Vector2<T>& a, const Vector2<U>& b)
{
    return Vector2<decltype(T() + U())>(a.x + b.x, a.y + b.y);
}

template<typename T>
auto operator +(const Vector2<T>& a, T v)
{
    return Vector2<T>(a.x + v, a.y + v);
}

template<typename T, typename U>
auto operator -(const Vector2<T>& a, const Vector2<U>& b)
{
    return Vector2<decltype(T() - U())>(a.x - b.x, a.y - b.y);
}

template<typename T>
auto operator -(const Vector2<T>& a, T v)
{
    return Vector2<T>(a.x - v, a.y - v);
}

template<typename T, typename U>
auto operator *(const Vector2<T>& v, U s)
{
    return Vector2<decltype(T() * U())>(v.x * s, v.y * s);
}

template<typename T, typename U>
auto operator /(const Vector2<T>& v, U s)
{
    return Vector2<decltype(T() / U())>(v.x / s, v.y / s);
}

template<typename T, typename U>
auto operator *(U s, const Vector2<T>& v)
{
    return Vector2<decltype( U() * T())>(v.x * s, v.y * s);
}

template<typename T, typename U>
auto operator /(U s, const Vector2<T>& v)
{
    return Vector2<decltype( U() / T())> (s / v.x, s / v.y);
}

template<typename T, typename U>
auto operator /(const Vector2<T>& a, const Vector2<U>& b)
{
    return Vector2<decltype(T() / T())>(a.x / b.x, a.y / b.y);
}

template<typename T>
auto operator ==(const Vector2<T>& a, const Vector2<T>& b)
{
    return a.x == b.x && a.y == b.y;
}

template<typename T>
auto operator !=(const Vector2<T>& a, const Vector2<T>& b)
{
    return a.x != b.x || a.y != b.y;
}

template<typename T>
float length(const Vector2<T>& v)
{
    return std::sqrt( v.x*v.x + v.y*v.y );
}

template<typename T>
Vector2<float> normalize(const Vector2<T>& v)
{
    assert(length(v) != 0.0f);
    return v / length(v);
}

template<typename T, typename U>
auto dot(const Vector2<T>& a, const Vector2<U>& b)
{
    return a.x * b.x + a.y + b.y;
}

template<typename T>
auto abs(const Vector2<T>& v)
{
    return Vector2<T>{ std::abs(v.x), std::abs(v.y) };
}

template<typename T>
auto clamp(const Vector2<T>& v, const Vector2<T>& min, const Vector2<T>& max)
{
    return Vector2<T>{ std::clamp<T>(v.x, min.x, max.x), std::clamp<T>(v.y, min.y, max.y)  };
}

template<typename T, typename U>
auto project(const Vector2<T>& a, const Vector2<U>& b)
{
    return (b * (dot(a, b) / dot(b, b)));
}

template<typename T, typename U>
auto reflect(const Vector2<T>& I, const Vector2<U>& N)
{
    return I - 2.0f * dot(I, N) * N;
}

template<typename T>
Vector2<float> linear(const Vector2<T>& a, const Vector2<T>& b, const Vector2<T>& p)
{
    auto dir = b - a;
    auto ap = p - a;
    auto dir_ap = dot(normalize(dir), ap);

    float l = length(dir);
    float frac = std::clamp<float>(dir_ap / l, 0.0f, 1.0f);

    return { frac, 1.0f - frac };
}

template<typename T>
auto mix(const Vector2<T>& a, const Vector2<T>& b, float t)
{
    t = std::clamp(t, 0.0f, 1.0f);
    return (1.0f - t) * a + t * b;
}

template<typename T>
auto min(const Vector2<T>& a, const Vector2<T>& b)
{
    return Vector2<T>{ std::min(a.x, b.x), std::min(a.y, b.y) };
}

template<typename T>
auto max(const Vector2<T>& a, const Vector2<T>& b)
{
    return Vector2<T>{ std::max(a.x, b.x), std::max(a.y, b.y) };
}

typedef Vector2<int> Vec2i;
typedef Vector2<float> Vec2;
typedef Vector2<std::uint8_t> Vec2u_8;
