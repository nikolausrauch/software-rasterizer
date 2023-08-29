#pragma once

#include "vector2.h"
#include "vector3.h"

#include <iostream>

template<typename T>
struct Vector4
{
    T x, y, z, w;

    Vector4()
        : x(0), y(0), z(0), w(0)
    {

    }

    Vector4(T v)
        : x(v), y(v), z(v), w(v)
    {

    }

    Vector4(T x, T y, T z, T w)
        : x(x), y(y), z(z), w(w)
    {

    }

    template<typename U>
    Vector4(const Vector3<U>& v, T w)
        : x(v.x), y(v.y), z(v.z), w(w)
    {

    }

    template<typename U>
    Vector4(const Vector2<U>& v, T z, T w)
        : x(v.x), y(v.y), z(z), w(w)
    {

    }

    template<typename U>
    Vector4(const Vector4<U>& a)
        : x(a.x), y(a.y), z(a.z), w(a.w)
    {

    }


    template<typename U>
    operator Vector2<U>() const { return Vector2<U>(x, y); }

    Vector4 operator -() const
    {
        return Vector4(-x, -y, -z, -w);
    }

    Vector4& operator *=(T s)
    {
        x *= s;
        y *= s;
        z *= s;
        w *= s;

        return *this;
    }

    Vector4& operator /=(T s)
    {
        assert(s != 0.0f);
        return *this *= (1.0 / s);
    }

    Vector4& operator /=(const Vector4& s)
    {
        assert(s.x != 0 && s.y != 0 && s.z != 0 && s.w != 0);
        x /= s.x;
        y /= s.y;
        z /= s.z;
        w /= s.w;

        return *this;
    }

    Vector4& operator +=(const Vector4& v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
        w += v.w;

        return *this;
    }

    Vector4& operator -=(const Vector4& v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        w -= v.w;

        return *this;
    }

    Vector4& operator +=(const T v)
    {
        x += v;
        y += v;
        z += v;
        w += v;

        return *this;
    }

    Vector4& operator -=(const T v)
    {
        x -= v;
        y -= v;
        z -= v;
        w -= v;

        return *this;
    }

    T& operator [](unsigned int i)
    {
        assert(i < 4);
        return ((&x)[i]);
    }

    const T& operator [](unsigned int i) const
    {
        assert(i < 4);
        return ((&x)[i]);
    }

private:
    friend std::ostream& operator<< (std::ostream& os, const Vector4<T>& vec)
    {
        return os << "{" << +vec.x << ", " << +vec.y << ", " << +vec.z << ", " << +vec.w <<  "}";
    }
};

template<typename T, typename U>
auto operator *(const Vector4<T>& a, const Vector4<U>& b)
{
    return Vector4<decltype(T() * U())>(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
}

template<typename T, typename U>
auto operator +(const Vector4<T>& a, const Vector4<U>& b)
{
    return Vector4<decltype(T() + U())>(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

template<typename T, typename U>
auto operator -(const Vector4<T>& a, const Vector4<U>& b)
{
    return Vector4<decltype(T() - U())>(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

template<typename T, typename U>
auto operator *(const Vector4<T>& v, U s)
{
    return Vector4<decltype(T() * U())>(v.x * s, v.y * s, v.z * s, v.w * s);
}

template<typename T, typename U>
auto operator /(const Vector4<T>& v, U s)
{
    return Vector4<decltype(T() / U())>(v.x / s, v.y / s, v.z / s, v.w / s);
}

template<typename T, typename U>
auto operator /(const Vector4<T>& a, const Vector4<U>& b)
{
    return Vector4<decltype(T() / U())>(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w);
}

template<typename T>
auto operator +(const Vector4<T>& a, const T v)
{
    return Vector4<T>(a.x + v, a.y + v, a.z + v, a.w + v);
}

template<typename T>
auto operator -(const Vector4<T>& a, const T v)
{
    return Vector4<T>(a.x - v, a.y - v, a.z - v, a.w - v);
}

template<typename T, typename U>
auto operator *(U s, const Vector4<T> &v)
{
    return Vector4<decltype(T() * U())>(v.x * s, v.y * s, v.z * s, v.w * s);
}

template<typename T, typename U>
auto operator /(U s, const Vector4<T> &v)
{
    return Vector4<decltype(T() / U())>(s / v.x, s / v.y, s / v.z, s / v.w);
}

template<typename T>
auto operator ==(const Vector4<T>& a, const Vector4<T>& b)
{
    return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}

template<typename T>
auto operator !=(const Vector4<T>& a, const Vector4<T>& b)
{
    return a.x != b.x && a.y != b.y && a.z != b.z && a.w != b.w;
}

template<typename T>
float length(const Vector4<T>& v)
{
    return std::sqrt(v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w);
}

template<typename T>
Vector4<float> normalize(const Vector4<T>& v)
{
    assert(length(v) != 0.0f);
    return v / length(v);
}

template<typename T>
auto abs(const Vector4<T>& v)
{
    return Vector4<T>{ std::abs(v.x), std::abs(v.y), std::abs(v.z), std::abs(v.w) };
}

template<typename T>
auto mix(const Vector4<T>& a, const Vector4<T>& b, float t)
{
    t = std::clamp(t, 0.0f, 1.0f);
    return (1.0f - t) * a + t * b;
}

template<typename T>
auto clamp(const Vector4<T>& v, const Vector4<T>& min, const Vector4<T>& max)
{
    return Vector4<T>{ std::clamp<T>(v.x, min.x, max.x), std::clamp<T>(v.y, min.y, max.y), std::clamp<T>(v.z, min.z, max.z), std::clamp<T>(v.w, min.w, max.w)   };
}

template<typename T, typename U>
auto min(const Vector4<T> &v, U s)
{
    typedef decltype(T() / U()) Type;
    return Vector4<Type>(std::min<Type>(v.x, s), std::min<Type>(v.y, s), std::min<Type>(v.z, s), std::min<Type>(v.w, s));
}

template<typename T, typename U>
auto max(const Vector4<T> &v, U s)
{
    typedef decltype(T() / U()) Type;
    return Vector4<Type>(std::max<Type>(v.x, s), std::max<Type>(v.y, s), std::max<Type>(v.z, s), std::max<Type>(v.w, s));
}

template<typename T>
Vector4<T> min(const Vector4<T>& a, const Vector4<T>& b)
{
    return
    {
        std::min(a.x, b.x),
        std::min(a.y, b.y),
        std::min(a.z, b.z),
        std::min(a.w, b.w)
    };
}


template<typename T>
Vector4<T> max(const Vector4<T>& a, const Vector4<T>& b)
{
    return
    {
        std::max(a.x, b.x),
        std::max(a.y, b.y),
        std::max(a.z, b.z),
        std::max(a.w, b.w)
    };
}

typedef Vector4<int> Vec4i;
typedef Vector4<float> Vec4;
typedef Vector4<std::uint8_t> Vec4u_8;
