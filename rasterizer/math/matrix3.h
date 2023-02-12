#pragma once

#include "vector3.h"

template<typename U> struct Matrix4;

template<typename T>
struct Matrix3
{
    T n[3][3];

    Matrix3()
    {
        n[0][0] = n[0][1] = n[0][2] = 0;
        n[1][0] = n[1][1] = n[1][2] = 0;
        n[2][0] = n[2][1] = n[2][2] = 0;
    }

    Matrix3(T n00, T n01, T n02,
            T n10, T n11, T n12,
            T n20, T n21, T n22)
    {
        n[0][0] = n00; n[0][1] = n10; n[0][2] = n20;
        n[1][0] = n01; n[1][1] = n11; n[1][2] = n21;
        n[2][0] = n02; n[2][1] = n12; n[2][2] = n22;
    }


    template<typename U>
    Matrix3(const Matrix4<U>& M)
    {
        n[0][0] = M(0,0);   n[0][1] = M(1,0);   n[0][2] = M(2,0);
        n[1][0] = M(0,1);   n[1][1] = M(1,1);   n[1][2] = M(2,1);
        n[2][0] = M(0,2);   n[2][1] = M(1,2);   n[2][2] = M(2,2);
    }

    Matrix3(const Vector3<T>& col0, const Vector3<T>& col1, const Vector3<T>& col2)
        : Matrix3(
            col0[0], col1[0], col2[0],
            col0[1], col1[1], col2[1],
            col0[2], col1[2], col2[2])
    {

    }

    T& operator ()(int i, int j)
    {
        assert(i < 3 && j < 3);
        return n[j][i];
    }

    const T& operator ()(int i, int j) const
    {
        assert(i < 3 && j < 3);
        return (n[j][i]);
    }

    Vector3<T>& operator [](int j)
    {
        assert(j < 3);
        return *reinterpret_cast< Vector3<T> *>(n[j]);
    }

    const Vector3<T>& operator [](int j) const
    {
        assert(j < 3);
        return *reinterpret_cast<const Vector3<T> *>(n[j]);
    }

    const T *ptr() const
    {
        return &(n[0][0]);
    }

    friend std::ostream& operator<<(std::ostream& os, const Matrix3<T>& M)
    {
        return os << M(0, 0) << " " << M(0, 1) << " " << M(0, 2) << "\n"
                  << M(1, 0) << " " << M(1, 1) << " " << M(1, 2) << "\n"
                  << M(2, 0) << " " << M(2, 1) << " " << M(2, 2);
    }



    template<typename t = T, typename std::enable_if_t<std::is_floating_point_v<t>, int> = 0>
    static Matrix3 identity()
    {
        return Matrix3(  1, 0, 0,
                         0, 1, 0,
                         0, 0, 1 );
    }

    template<typename t = T, typename std::enable_if_t<std::is_floating_point_v<t>, int> = 0>
    static Matrix3 scale(T sx, T sy, T sz)
    {
        return Matrix3( sx,  0.0f, 0.0f,
                        0.0f,  sy,  0.0f,
                        0.0f, 0.0f,  sz);
    }

    template<typename t = T, typename std::enable_if_t<std::is_floating_point_v<t>, int> = 0>
    static Matrix3 rotationX(T r)
    {
        float c = std::cos(r);
        float s = std::sin(r);

        return Matrix3( 1.0f, 0.0f, 0.0f,
                        0.0f,  c,   -s,
                        0.0f,  s,    c  );
    }

    template<typename t = T, typename std::enable_if_t<std::is_floating_point_v<t>, int> = 0>
    static Matrix3 rotationY(T r)
    {
        float c = std::cos(r);
        float s = std::sin(r);

        return Matrix3(  c,   0.0f,  s,
                        0.0f, 1.0f, 0.0f,
                        -s,   0.0f,  c  );
    }

    template<typename t = T, typename std::enable_if_t<std::is_floating_point_v<t>, int> = 0>
    static Matrix3 rotationZ(T r)
    {
        float c = std::cos(r);
        float s = std::sin(r);

        return Matrix3(  c,   -s,    0.0f,
                         s,    c,    0.0f,
                         0.0f, 0.0f, 1.0f);
    }

    template<typename t = T, typename std::enable_if_t<std::is_floating_point_v<t>, int> = 0>
    static Matrix3 rotation(T r, const Vector3<T> &a)
    {
        float c = std::cos(r);
        float s = std::sin(r);
        float d = 1.0F - c;

        float x = a.x * d;
        float y = a.y * d;
        float z = a.z * d;
        float axay = x * a.y;
        float axaz = x * a.z;
        float ayaz = y * a.z;

        return (Matrix3(    c + x * a.x,  axay - s * a.z,  axaz + s * a.y,
                         axay + s * a.z,     c + y * a.y,  ayaz - s * a.x,
                         axaz - s * a.y,  ayaz + s * a.x,     c + z * a.z));
    }
};


template<typename T, typename U>
auto operator *(const Matrix3<T> &A, const Matrix3<U> &B)
{
    return Matrix3<decltype( T() * U() )>
                    (A(0,0) * B(0,0) + A(0,1) * B(1,0) + A(0,2) * B(2,0),
                     A(0,0) * B(0,1) + A(0,1) * B(1,1) + A(0,2) * B(2,1),
                     A(0,0) * B(0,2) + A(0,1) * B(1,2) + A(0,2) * B(2,2),

                     A(1,0) * B(0,0) + A(1,1) * B(1,0) + A(1,2) * B(2,0),
                     A(1,0) * B(0,1) + A(1,1) * B(1,1) + A(1,2) * B(2,1),
                     A(1,0) * B(0,2) + A(1,1) * B(1,2) + A(1,2) * B(2,2),

                     A(2,0) * B(0,0) + A(2,1) * B(1,0) + A(2,2) * B(2,0),
                     A(2,0) * B(0,1) + A(2,1) * B(1,1) + A(2,2) * B(2,1),
                     A(2,0) * B(0,2) + A(2,1) * B(1,2) + A(2,2) * B(2,2));
}

template<typename T, typename U>
auto operator *(const Matrix3<T>& M, const Vector3<U>& v)
{
    return Vector3<decltype( T() * U() )>
                   ( M(0,0) * v.x + M(0,1) * v.y + M(0,2) * v.z,
                     M(1,0) * v.x + M(1,1) * v.y + M(1,2) * v.z,
                     M(2,0) * v.x + M(2,1) * v.y + M(2,2) * v.z);
}

template<typename T, typename std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
Matrix3<T> inverse(const Matrix3<T>& M)
{
    const auto& a = M[0];
    const auto& b = M[1];
    const auto& c = M[2];

    Vector3 r0 = cross(b, c);
    Vector3 r1 = cross(c, a);
    Vector3 r2 = cross(a, b);

    float invDet = 1.0f / dot(r2, c);

    return Matrix3<T>
                    (r0.x * invDet, r0.y * invDet, r0.z * invDet,
                     r1.x * invDet, r1.y * invDet, r1.z * invDet,
                     r2.x * invDet, r2.y * invDet, r2.z * invDet);
}

typedef Matrix3<int> Mat3i;
typedef Matrix3<float> Mat3;
