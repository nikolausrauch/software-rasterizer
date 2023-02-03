#pragma once

#include "texture.h"

#include <algorithm>

template<typename T>
struct Sampler;

template<typename T>
T texture(const Sampler<T>& sampler, const Vec2& uv);

enum class eFilter
{
    NEAREST,
    LINEAR
};

template<typename T>
struct Sampler
{
    Sampler() : m_texture(nullptr) {}

    Sampler& operator =(Texture<T>& texture)
    {
        m_texture = &texture;
        return *this;
    }

    Sampler& operator =(const Texture<T>& texture)
    {
        m_texture = &texture;
    }


public:
    eFilter filter = eFilter::NEAREST;

public:
    const Texture<T>* m_texture;

    friend T texture<>(const Sampler<T>& sampler, const Vec2& uv);
};

template<typename T>
T texture(const Sampler<T>& sampler, const Vec2& uv)
{
    assert(sampler.m_texture != nullptr);

    int width = sampler.m_texture->width();
    int height = sampler.m_texture->height();

    if(sampler.filter == eFilter::NEAREST)
    {
        Vec2i coord(std::round(uv.x * width), std::round(uv.y * height));
        coord.x = std::clamp(coord.x, 0, width);
        coord.y = std::clamp(coord.y, 0, height);

        return (*sampler.m_texture)(coord.x, coord.y);
    }
    else if(sampler.filter == eFilter::LINEAR)
    {
        Vec2 upper(std::ceil(uv.x * width), std::ceil(uv.y * height));
        Vec2 lower(std::floor(uv.x * width), std::floor(uv.y * height));

        Vec2 coord = Vec2(uv.x * width, uv.y * height) - lower;

        float iwidth = upper.x - lower.x;
        float iheight = upper.y - lower.y;

        int width = sampler.m_texture->width();
        int height = sampler.m_texture->height();
        upper.x = std::clamp<float>(upper.x, 0, width);
        upper.y = std::clamp<float>(upper.y, 0, height);
        lower.x = std::clamp<float>(lower.x, 0, width);
        lower.y = std::clamp<float>(lower.y, 0, height);

        auto color_00 = (*sampler.m_texture)(lower.x, lower.y);
        auto color_10 = (*sampler.m_texture)(upper.x, lower.y);
        auto color_01 = (*sampler.m_texture)(lower.x, upper.y);
        auto color_11 = (*sampler.m_texture)(upper.x, upper.y);

        float t_x = coord.x / iwidth;
        auto color_0 = color_00 * t_x + color_10 * (1.0f - t_x);
        auto color_1 = color_01 * t_x + color_11 * (1.0f - t_x);

        float t_y = coord.y / iheight;
        return color_0 * t_y + color_1 * (1.0f - t_y);
    }

    return T();
}
