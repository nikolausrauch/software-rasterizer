#pragma once

#include "texture.h"
#include "math/vector4.h"
#include "detail/tuple_helper.h"

#include <tuple>


template<typename... Targets>
struct Framebuffer
{
    using TargetStorage = std::tuple< Texture<Targets>... >;
    using TargetFragments = std::tuple< Targets&... >;

    template<typename T> using HasTarget = std::enable_if_t< detail::tuple_has_type<T, TargetStorage>::value >;

    static constexpr bool has_color = detail::tuple_has_type< Texture<RGBA8>, TargetStorage>::value;
    static constexpr bool has_depth = detail::tuple_has_type< Texture<Depth>, TargetStorage>::value;


    Framebuffer(unsigned int width, unsigned int height)
        : m_width(width), m_height(height), m_targets(Texture<Targets>(width, height)...)
    {

    }

    template<std::size_t idx>
    auto& target()
    {
        return std::get<idx>(m_targets);
    }

    TargetStorage& targets()
    {
        return m_targets;
    }

    TargetFragments targets(int x, int y)
    {
        return detail::tuple_construct([x, y](auto& ts) -> auto& { return ts(x,y); }, m_targets);
    }

    template<typename T = Texture<RGBA8>, typename = HasTarget<T>> Texture<RGBA8>& color()
    {
        return std::get< detail::tuple_index<Texture<RGBA8>, TargetStorage>::index >(m_targets);
    }

    template<typename T = Texture<Depth>, typename = HasTarget<T>> Texture<Depth>& depth()
    {
        return std::get< detail::tuple_index<Texture<Depth>, TargetStorage>::index >(m_targets);
    }

    void clear(const RGBA8& color)
    {
        detail::tuple_iter([color](auto& target)
        {
            if constexpr (std::is_same_v<typename std::remove_reference<decltype(target)>::type, Texture<RGBA8>>)
            {
                target.fill(color);
            }

            if constexpr (std::is_same_v<typename std::remove_reference<decltype(target)>::type, Texture<Depth>>)
            {
                target.fill({ std::numeric_limits<float>::max() });
            }

        }, m_targets);
    }

    void clear(const Vec4& color)
    {
        clear(RGBA8(color * 255));
    }

    void clear()
    {
        clear(RGBA8());
    }

private:
    int m_width;
    int m_height;
    TargetStorage m_targets;
};

using DefaultFramebuffer = Framebuffer<RGBA8, Depth>;
