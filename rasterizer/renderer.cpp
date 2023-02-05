#include "renderer.h"

Renderer::Renderer(unsigned int width, unsigned int height)
    : m_framebuffer(width, height),
      m_options{ {0, 0, static_cast<float>(width), static_cast<float>(height)}, true }
{
    m_framebuffer.clear(RGBA8(0, 0, 0, 0));
}

DefaultFramebuffer &Renderer::framebuffer()
{
    return m_framebuffer;
}

Renderer::Options& Renderer::options()
{
    return m_options;
}
