#pragma once

#include "texture.h"
#include "math/vector4.h"


#include <vector>
#include <string>


typedef Vector4<std::uint8_t> Color;
typedef float Depth;

struct Framebuffer
{
    Framebuffer(unsigned int width, unsigned int height);

    void clear(const Color& color);
    void clear(const Vec4& color);

    Texture<Color>& color();
    Texture<Depth>& depth();

private:
    int m_width;
    int m_height;

    Texture<Color> m_colorBuffer;
    Texture<Depth> m_depthBuffer;
};
