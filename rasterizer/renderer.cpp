#include "renderer.h"

Renderer::Renderer(unsigned int width, unsigned int height)
    : m_framebuffer(width, height),
      m_options{ {0, 0, static_cast<float>(width), static_cast<float>(height)}, true }
{
    m_framebuffer.clear(Color(0, 0, 0, 0));
}

Framebuffer &Renderer::framebuffer()
{
    return m_framebuffer;
}
