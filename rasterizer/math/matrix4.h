#pragma once

#include "matrix3.h"
#include "vector4.h"

template<typename T>
struct Matrix4
{
    T n[4][4];

    Matrix4()
    {
        n[0][0] = n[0][1] = n[0][2] = n[0][3] = 0;
        n[1][0] = n[1][1] = n[1][2] = n[1][3] = 0;
        n[2][0] = n[2][1] = n[2][2] = n[2][3] = 0;
        n[3][0] = n[3][1] = n[3][2] = n[3][3] = 0;
    }

    Matrix4(T n00, T n01, T n02, T n03,
            T n10, T n11, T n12, T n13,
            T n20, T n21, T n22, T n23,
            T n30, T n31, T n32, T n33)
    {
        n[0][0] = n00; n[0][1] = n10; n[0][2] = n20; n[0][3] = n30;
        n[1][0] = n01; n[1][1] = n11; n[1][2] = n21; n[1][3] = n31;
        n[2][0] = n02; n[2][1] = n12; n[2][2] = n22; n[2][3] = n32;
        n[3][0] = n03; n[3][1] = n13; n[3][2] = n23; n[3][3] = n33;
    }

    template<typename U>
    Matrix4(const Matrix3<U>& M)
    {
        n[0][0] = M(0,0);   n[0][1] = M(1,0);   n[0][2] = M(2,0);   n[0][3] = 0;
        n[1][0] = M(0,1);   n[1][1] = M(1,1);   n[1][2] = M(2,1);   n[1][3] = 0;
        n[2][0] = M(0,2);   n[2][1] = M(1,2);   n[2][2] = M(2,2);   n[2][3] = 0;
        n[3][0] = 0;        n[3][1] = 0;        n[3][2] = 0;        n[3][3] = 1;
    }

    T& operator ()(int i, int j)
    {
        assert(i < 4 && j < 4);
        return n[j][i];
    }

    Vector4<T>& operator [](int j)
    {
        assert(j < 4);
        return *reinterpret_cast<Vector4<T> *>(n[j]);
    }

    const T* ptr() const
    {
        return &(n[0][0]);
    }

    const Vector4<T>& operator [](int j) const
    {
        assert(j < 4);
        return *reinterpret_cast<const Vector4<T> *>(n[j]);
    }

    const T& operator ()(int i, int j) const
    {
        assert(i < 4 && j < 4);
        return n[j][i];
    }

    friend std::ostream& operator<<(std::ostream& os, const Matrix4<T>& M)
    {
        return os << M(0, 0) << " " << M(0, 1) << " " << M(0, 2) << " " << M(0, 3) << "\n"
                  << M(1, 0) << " " << M(1, 1) << " " << M(1, 2) << " " << M(1, 3) << "\n"
                  << M(2, 0) << " " << M(2, 1) << " " << M(2, 2) << " " << M(2, 3) << "\n"
                  << M(3, 0) << " " << M(3, 1) << " " << M(3, 2) << " " << M(3, 3);
    }


    static Matrix4 identity()
    {
        return Matrix4(1, 0, 0, 0,
                        0, 1, 0, 0,
                        0, 0, 1, 0,
                        0, 0, 0, 1);
    }

    template<typename t = T, typename std::enable_if_t<std::is_floating_point_v<t>, int> = 0>
    static Matrix4 scale(T sx, T sy, T sz)
    {
        return Matrix4(Matrix3<float>::scale(sx, sy, sz));
    }

    template<typename t = T, typename std::enable_if_t<std::is_floating_point_v<t>, int> = 0>
    static Matrix4 rotationX(T r)
    {
        return Matrix4(Matrix3<float>::rotationX(r));
    }

    template<typename t = T, typename std::enable_if_t<std::is_floating_point_v<t>, int> = 0>
    static Matrix4 rotationY(T r)
    {
        return Matrix4(Matrix3<T>::rotationY(r));
    }

    template<typename t = T, typename std::enable_if_t<std::is_floating_point_v<t>, int> = 0>
    static Matrix4 rotationZ(T r)
    {
        return Matrix4(Matrix3<T>::rotationZ(r));
    }

    template<typename t = T, typename std::enable_if_t<std::is_floating_point_v<t>, int> = 0>
    static Matrix4 rotation(T r, const Vector3<T> &a)
    {
        return Matrix4(Matrix3<T>::rotation(r, a));
    }

    template<typename t = T, typename std::enable_if_t<std::is_floating_point_v<t>, int> = 0>
    static Matrix4 translation(const Vector3<T> &v)
    {
        return Matrix4(1, 0, 0, v.x,
                        0, 1, 0, v.y,
                        0, 0, 1, v.z,
                        0, 0, 0,  1  );
    }

    template<typename t = T, typename std::enable_if_t<std::is_floating_point_v<t>, int> = 0>
    static Matrix4 perspective(T fov, T aspect, T nearPlane, T farPlane)
    {
        float f = 1.0f / std::tan(0.5 * fov);
        float c1 = -(farPlane + nearPlane) / (farPlane - nearPlane);
        float c2 = -(2.0 * farPlane * nearPlane) / (farPlane - nearPlane);

        return Matrix4(f/aspect,   0,  0,  0,
                        0,         f,  0,  0,
                        0,         0,  c1, c2,
                        0,         0,  -1,  0);
    }

    template<typename t = T, typename std::enable_if_t<std::is_floating_point_v<t>, int> = 0>
    static Matrix4 ortho(T left, T bottom, T right, T top, T near, T far)
    {
        return Matrix4(
                    2.0f / (right - left),  0.0f,                   0.0f,                   -(right+left)/(right-left),
                    0.0f,                   2.0f / (top - bottom),  0.0f,                   -(top+bottom)/(top-bottom),
                    0.0f,                   0.0f,                   -2.0f / (far - near),   -(far+near)/(far-near),
                    0.0f,                   0.0f,                   0.0f,                   1.0f
                    );
    }
};

template<typename T, typename U>
auto operator *(const Matrix4<T>& A, const Matrix4<U>& B)
{
    return Matrix4<decltype( T() * U() )>
                   (A(0,0) * B(0,0) + A(0,1) * B(1,0) + A(0,2) * B(2,0) + A(0,3) * B(3,0),
                    A(0,0) * B(0,1) + A(0,1) * B(1,1) + A(0,2) * B(2,1) + A(0,3) * B(3,1),
                    A(0,0) * B(0,2) + A(0,1) * B(1,2) + A(0,2) * B(2,2) + A(0,3) * B(3,2),
                    A(0,0) * B(0,3) + A(0,1) * B(1,3) + A(0,2) * B(2,3) + A(0,3) * B(3,3),

                    A(1,0) * B(0,0) + A(1,1) * B(1,0) + A(1,2) * B(2,0) + A(1,3) * B(3,0),
                    A(1,0) * B(0,1) + A(1,1) * B(1,1) + A(1,2) * B(2,1) + A(1,3) * B(3,1),
                    A(1,0) * B(0,2) + A(1,1) * B(1,2) + A(1,2) * B(2,2) + A(1,3) * B(3,2),
                    A(1,0) * B(0,3) + A(1,1) * B(1,3) + A(1,2) * B(2,3) + A(1,3) * B(3,3),

                    A(2,0) * B(0,0) + A(2,1) * B(1,0) + A(2,2) * B(2,0) + A(2,3) * B(3,0),
                    A(2,0) * B(0,1) + A(2,1) * B(1,1) + A(2,2) * B(2,1) + A(2,3) * B(3,1),
                    A(2,0) * B(0,2) + A(2,1) * B(1,2) + A(2,2) * B(2,2) + A(2,3) * B(3,2),
                    A(2,0) * B(0,3) + A(2,1) * B(1,3) + A(2,2) * B(2,3) + A(2,3) * B(3,3),

                    A(3,0) * B(0,0) + A(3,1) * B(1,0) + A(3,2) * B(2,0) + A(3,3) * B(3,0),
                    A(3,0) * B(0,1) + A(3,1) * B(1,1) + A(3,2) * B(2,1) + A(3,3) * B(3,1),
                    A(3,0) * B(0,2) + A(3,1) * B(1,2) + A(3,2) * B(2,2) + A(3,3) * B(3,2),
                    A(3,0) * B(0,3) + A(3,1) * B(1,3) + A(3,2) * B(2,3) + A(3,3) * B(3,3));
}

template<typename T, typename U>
auto operator *(const Matrix4<T>& M, const Vector4<U>& v)
{
    return Vector4<decltype( T() * U() )>
                   (M(0,0) * v[0] + M(0,1) * v[1] + M(0,2) * v[2] + M(0,3) * v[3],
                    M(1,0) * v[0] + M(1,1) * v[1] + M(1,2) * v[2] + M(1,3) * v[3],
                    M(2,0) * v[0] + M(2,1) * v[1] + M(2,2) * v[2] + M(2,3) * v[3],
                    M(3,0) * v[0] + M(3,1) * v[1] + M(3,2) * v[2] + M(3,3) * v[3]);
}

template<typename T, typename std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
auto inverse(const Matrix4<T> &M)
{
    const auto& a = reinterpret_cast<const Vector3<T>&>(M[0]);
    const auto& b = reinterpret_cast<const Vector3<T>&>(M[1]);
    const auto& c = reinterpret_cast<const Vector3<T>&>(M[2]);
    const auto& d = reinterpret_cast<const Vector3<T>&>(M[3]);

    const T& x = M(3,0);
    const T& y = M(3,1);
    const T& z = M(3,2);
    const T& w = M(3,3);

    auto s = cross(a, b);
    auto t = cross(c, d);
    auto u = a * y - b * x;
    auto v = c * w - d * z;

    auto invDet = 1.0f / (dot(s, v) + dot(t, u));
    s *= invDet;
    t *= invDet;
    u *= invDet;
    v *= invDet;

    auto r0 = cross(b, v) + t * y;
    auto r1 = cross(v, a) - t * x;
    auto r2 = cross(d, u) + s * w;
    auto r3 = cross(u, c) - s * z;

    return Matrix4<T>
                    (r0.x, r0.y, r0.z, -dot(b, t),
                     r1.x, r1.y, r1.z,  dot(a, t),
                     r2.x, r2.y, r2.z, -dot(d, s),
                     r3.x, r3.y, r3.z,  dot(c, s));
}

typedef Matrix4<int> Mat4i;
typedef Matrix4<float> Mat4;
