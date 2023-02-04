#pragma once

#include "detail/test_member.h"

#include "math/vector4.h"

#include <functional>

template<typename Vertex, typename Varying, typename Uniforms>
struct Program
{
    static_assert (detail::has_member<Varying>::position::value, "Output of Vertex Stage needs Vec4 position!");
    static_assert (detail::has_member<Varying>::_reflect::value, "Output of Vertex Stage needs interpolated positional values. Did you forget to set VARYING(position) macro? ");

    typedef std::function< void (const Uniforms& uniforms, const Vertex& in, Varying& out) > VertexShader;
    typedef std::function< void (const Uniforms& uniforms, const Varying& in, Vec4& out) > FragmentShader;

    void onVertex(const VertexShader& shader) { m_vertShader = shader; }
    void onFragment(const FragmentShader& shader) { m_fragShader = shader; }
    Uniforms& uniforms() { return m_uniforms; }

private:
    VertexShader m_vertShader;
    FragmentShader m_fragShader;
    Uniforms m_uniforms;

    friend struct Renderer;
};
