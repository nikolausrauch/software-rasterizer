#pragma once

#include "base.h"

#include "vector2.h"

template<typename T>
struct Rectangle
{
    Vector2<T> min;
    Vector2<T> max;

    Rectangle(T min_x, T min_y, T max_x, T max_y)
        : min(min_x, min_y), max(max_x, max_y)
    {

    }

    Rectangle(const Vector2<T>& min, const Vector2<T>& max)
        : min(min), max(max)
    {

    }

    Rectangle(const Vector2<T>& p_0, const Vector2<T>& p_1, const Vector2<T>& p_2)
        : Rectangle(std::numeric_limits<T>::max(), std::numeric_limits<T>::max(), std::numeric_limits<T>::min(), std::numeric_limits<T>::min())
    {
        merge(p_0);
        merge(p_1);
        merge(p_2);
    }

    template<typename U>
    bool contains(const Vector2<U>& point)
    {
        return (point.x >= min.x && point.x < max.x ) &&
               (point.y >= min.y && point.y < max.y );
    }

    template<typename U>
    void merge(const Vector2<U>& p)
    {
        min.x = std::min<T>(min.x, p.x);
        min.y = std::min<T>(min.y, p.y);

        max.x = std::max<T>(max.x, p.x);
        max.y = std::max<T>(max.y, p.y);
    }

    template<typename U>
    void merge(const Vector2<T>& p_0, const Vector2<T>& p_1, const Vector2<T>& p_2)
    {
        merge(p_0);
        merge(p_1);
        merge(p_2);
    }

    template<typename U>
    void clamp(const Rectangle<U>& border, T pad_min = 0, T pad_max = 0)
    {
        min.x = std::max<T>(border.min.x + pad_min, min.x);
        min.y = std::max<T>(border.min.y + pad_min, min.y);

        max.x = std::min<T>(border.max.x + pad_max, max.x);
        max.y = std::min<T>(border.max.y + pad_max, max.y);
    }

    friend std::ostream& operator<<(std::ostream& os, const Rectangle<T>& rec)
    {
        return os << "{" << rec.min.x << ", " << rec.min.y << " | " << rec.max.x << ", " << rec.max.y << "}";
    }
};

template<typename T>
Vector2<T> clamp(const Rectangle<T>& border, const Vector2<T>& point, T pad_min = 0, T pad_max = 0)
{
    return { std::clamp<T>(point.x, border.min.x + pad_min, border.max.x + pad_max), std::clamp<T>(point.y, border.min.y + pad_min, border.max.y + pad_max) };
}

typedef Rectangle<int> Recti;
typedef Rectangle<float> Rectf;
