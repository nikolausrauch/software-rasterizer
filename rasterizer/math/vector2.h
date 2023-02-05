#pragma once

#include "base.h"

#include <cassert>
#include <iostream>

template<typename T> struct Vector4;
template<typename T> struct Vector3;

template<typename T>
struct Vector2
{
    T x, y;

    Vector2(T x = 0, T y = 0)
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

    Vector2& operator/=(T s)
    {
        assert(s != 0.0f);
        return *this *= (1.0 / s);
    }

    Vector2& operator+=(const Vector2 &v)
    {
        x += v.x;
        y += v.y;
        return *this;
    }

    Vector2& operator-=(const Vector2 &v)
    {
        x -= v.x;
        y -= v.y;
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
        return os << "{" << vec.x << ", " << vec.y <<  "}";
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

template<typename T, typename U>
auto operator -(const Vector2<T>& a, const Vector2<U>& b)
{
    return Vector2<decltype(T() - U())>(a.x - b.x, a.y - b.y);
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

typedef Vector2<int> Vec2i;
typedef Vector2<float> Vec2;

//inline Vector2 project(const Vector2& a, const Vector2& b)
//{
//    return (b * (dot(a, b) / dot(b, b)));
//}

//inline Vector2 reject(const Vector2& a, const Vector2& b)
//{
//    return (a - b * (dot(a, b) / dot(b, b)));
//}
