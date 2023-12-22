#pragma once

#include "texture.h"

#include <algorithm>

template<typename T> struct Sampler;
template<typename T> T texture(const Sampler<T>& sampler, const Vec2& uv);


enum class eFilter
{
    NEAREST,
    LINEAR,
    NEAREST_MIPMAP_LINEAR,
    LINEAR_MIPMAP_NEAREST,
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

    Sampler(Texture<T>& texture) : m_texture(&texture) {}

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
T sample_texture(const TextureStorage<T>& textureStorage, const Vec2& uv, eWrap wrap, eFilter filter)
{
    /* map based on wrap option to new uv coordinates */
    auto proj_uv = uv;
    if(wrap == eWrap::CLAMP_EDGE)
    {
        proj_uv = clamp(uv, {0.0f, 0.0f}, {1.0f, 1.0f});
    }
    else if(wrap == eWrap::REPEAT)
    {
        proj_uv = { std::fmod<float>(uv.x, 1.0f), std::fmod<float>(uv.y, 1.0f) };
        proj_uv = { std::fmod<float>(1.0 + proj_uv.x, 1.0f), std::fmod<float>(1.0 + proj_uv.y, 1.0f) };
    }

    /* query texture data with filter */
    int width = textureStorage.width();
    int height = textureStorage.height();

    if(filter == eFilter::NEAREST || filter == eFilter::LINEAR_MIPMAP_NEAREST)
    {
        Vec2i coord(std::round(proj_uv.x * width - 0.5f), std::round(proj_uv.y * height - 0.5f) );
        coord.x = std::clamp(coord.x, 0, width - 1);
        coord.y = std::clamp(coord.y, 0, height - 1);

        return textureStorage(coord.x, coord.y);
    }
    else if(filter == eFilter::LINEAR || filter == eFilter::NEAREST_MIPMAP_LINEAR)
    {
        /* TODO: apply wrapping for interpolation coords  */
        Vec2 coord = Vec2(proj_uv.x * width - 0.5f, proj_uv.y * height - 0.5f);
        Vec2 lower(int(coord.x), int(coord.y));
        Vec2 upper(lower.x + 1, lower.y + 1);

        upper.x = std::clamp<float>(upper.x, 0, width - 1);
        upper.y = std::clamp<float>(upper.y, 0, height - 1);

        coord = coord - lower;

        auto color_00 = textureStorage(lower.x, lower.y);
        auto color_10 = textureStorage(upper.x, lower.y);
        auto color_01 = textureStorage(lower.x, upper.y);
        auto color_11 = textureStorage(upper.x, upper.y);

        auto color_0 = color_00 * (1.0f - coord.x) + color_10 * coord.x;
        auto color_1 = color_01 * (1.0f - coord.x) + color_11 * coord.x;

        return color_0 * (1.0f - coord.y) + color_1 * coord.y;
    }

    return T();
}

template<typename T>
T texture(const Sampler<T>& sampler, const Vec2& uv)
{
    assert(sampler.m_texture != nullptr);
    return sample_texture(sampler.m_texture->mipmaps().front(), uv, sampler.wrap, sampler.filter);
}

template<typename T>
T textureLod(const Sampler<T>& sampler, const Vec2& uv, float level)
{
    float sample_level = std::clamp(level, 0.0f, static_cast<float>(sampler.m_texture->num_mipmaps() - 1));
    if(sampler.filter == eFilter::NEAREST || sampler.filter == eFilter::NEAREST_MIPMAP_LINEAR)
    {
        sample_level = std::round(sample_level);
        return sample_texture(sampler.m_texture->mipmaps().at(sample_level), uv, sampler.wrap, sampler.filter);
    }
    else
    {
        float floor_level = std::floor(sample_level);
        float ceil_level = std::ceil(sample_level);
        float weight = ceil_level - sample_level;

        auto floor_sample = sample_texture(sampler.m_texture->mipmaps().at(floor_level), uv, sampler.wrap, sampler.filter);
        auto ceil_sample = sample_texture(sampler.m_texture->mipmaps().at(ceil_level), uv, sampler.wrap, sampler.filter);
        return weight * ceil_sample + (1.0f - weight) * floor_sample;
    }
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
