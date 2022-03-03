#include "framebuffer.h"

#include <stb_image/stb_image_write.h>

Framebuffer::Framebuffer(unsigned int width, unsigned int height)
    : m_width(width), m_height(height), m_colorBuffer(width, height), m_depthBuffer(width, height)
{

}

void Framebuffer::clear(const Color &color)
{
    m_colorBuffer.fill(color);
    m_depthBuffer.fill(std::numeric_limits<float>::max());
}

void Framebuffer::clear(const Vec4& color)
{
    m_colorBuffer.fill(color * 255);
    m_depthBuffer.fill(std::numeric_limits<float>::max());
}


Texture<Color> &Framebuffer::color()
{
    return m_colorBuffer;
}

Texture<Depth> &Framebuffer::depth()
{
    return m_depthBuffer;
}
