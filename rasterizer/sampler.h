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

enum class eWrap
{
    REPEAT,
    CLAMP_EDGE
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
    eFilter filter = eFilter::LINEAR;
    eWrap wrap = eWrap::CLAMP_EDGE;

public:
    const Texture<T>* m_texture;

    friend T texture<>(const Sampler<T>& sampler, const Vec2& uv);
};

template<typename T>
T texture(const Sampler<T>& sampler, const Vec2& uv)
{
    assert(sampler.m_texture != nullptr);

    /* map based on wrap option to new uv coordinates */
    auto proj_uv = uv;
    if(sampler.wrap == eWrap::CLAMP_EDGE)
    {
        proj_uv = clamp(uv, {0.0f, 0.0f}, {1.0f, 1.0f});
    }
    else if(sampler.wrap == eWrap::REPEAT)
    {
        proj_uv = { std::fmod<float>(uv.x, 1.0f), std::fmod<float>(uv.y, 1.0f) };
        proj_uv = { std::fmod<float>(1.0 + proj_uv.x, 1.0f), std::fmod<float>(1.0 + proj_uv.y, 1.0f) };
    }


    /* query texture data with filter */
    int width = sampler.m_texture->width();
    int height = sampler.m_texture->height();

    if(sampler.filter == eFilter::NEAREST)
    {
        Vec2i coord(std::floor(proj_uv.x * (width - 1) ), std::floor(proj_uv.y * (height - 1)) );
        coord.x = std::clamp(coord.x, 0, width - 1);
        coord.y = std::clamp(coord.y, 0, height - 1);

        return (*sampler.m_texture)(coord.x, coord.y);
    }
    else if(sampler.filter == eFilter::LINEAR)
    {
        /* TODO: apply wrapping for interpolation coords  */

        Vec2 coord = Vec2(proj_uv.x * (width - 1), proj_uv.y * (height - 1));
        Vec2 upper(std::ceil(coord.x), std::ceil(coord.y));
        Vec2 lower(std::floor(coord.x), std::floor(coord.y));

        coord = coord - lower;

        auto color_00 = (*sampler.m_texture)(lower.x, lower.y);
        auto color_10 = (*sampler.m_texture)(upper.x, lower.y);
        auto color_01 = (*sampler.m_texture)(lower.x, upper.y);
        auto color_11 = (*sampler.m_texture)(upper.x, upper.y);

        auto color_0 = color_00 * coord.x + color_10 * (1.0f - coord.x);
        auto color_1 = color_01 * coord.x + color_11 * (1.0f - coord.x);

        return color_0 * coord.y + color_1 * (1.0f - coord.y);
    }

    return T();
}

template<typename T>
Vec2i textureSize( const Sampler<T>& sampler )
{
    return Vec2i( sampler.m_texture->width(), sampler.m_texture->height() );
}

template<typename T>
Vec2 textureInvSize( const Sampler<T>& sampler )
{
    return Vec2( 1.0f / sampler.m_texture->width(), 1.0f / sampler.m_texture->height() );
}
