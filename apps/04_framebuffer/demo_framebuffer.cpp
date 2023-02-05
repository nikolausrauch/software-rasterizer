#include "sampler.h"
#include <cstdlib>

#include <renderer.h>
#include <math/matrix4.h>

/* vertex, fragment data for triangle pass */
namespace triangle
{

struct Vertex
{
    Vec3 pos;
    Vec3 color;
};

struct Varying
{
    Vec4 position;
    Vec3 color;

    VARYING(position, color);
};

struct Uniforms
{

};

}


/* vertex, fragment data for fullscreen quad pass */
namespace quad
{

struct Vertex
{
    Vec3 pos;
    Vec2 uv;
};

struct Varying
{
    Vec4 position;
    Vec2 uv;

    VARYING(position, uv);
};

struct Uniforms
{
    Sampler<RGBA8> color;
};

}


int main(int argc, char** argv)
{
    /* "offscreen" framebuffer */
    Framebuffer<RGBA8> framebuffer(480, 480);


    /*========== Setup Shader Program (1. render pass to custom framebuffer) ========*/
    Program<triangle::Vertex, triangle::Varying, triangle::Uniforms, Framebuffer<RGBA8>> program_triangle;
    program_triangle.onVertex([](const auto& uniform, const auto& in, auto& out)
    {
        out.position = Vec4(in.pos, 1.0f);
        out.color = in.color;
    });

    program_triangle.onFragment([](const auto& uniform, const auto& in, auto& out)
    {
        auto& [out_color] = out; /* or std::get<0>(out) */
        out_color = RGBA8(255.0f * Vec4(in.color, 1.0f));
    });

    /*========== Setup Shader Program (2. render pass) ========*/
    Program<quad::Vertex, quad::Varying, quad::Uniforms> program_quad;
    program_quad.onVertex([](const auto& uniform, const auto& in, auto& out)
    {
        out.position = Vec4(in.pos, 1.0f);
        out.uv = in.uv;
    });

    program_quad.onFragment([](const auto& uniform, const auto& in, auto& out)
    {
        out = texture(uniform.color, in.uv) / 255.0f;
    });

    /* use framebuffer of first render pass in second shader */
    program_quad.uniforms().color.filter = eFilter::NEAREST;
    program_quad.uniforms().color = framebuffer.target<0>();


    /*========== Setup Buffer Data ========*/
    Buffer<triangle::Vertex> buffer_triangle;
    buffer_triangle.primitive = ePrimitive::TRIANGLES;
    buffer_triangle.vertices =  { { {-0.5, -0.5, 0.5}, {1.0, 0.0, 0.0} },
                                  { { 0.5, -0.5, 0.5}, {0.0, 1.0, 0.0} },
                                  { { 0.0,  0.5, 0.5}, {0.0, 0.0, 1.0} } };

    Buffer<quad::Vertex> buffer_quad;
    buffer_quad.primitive = ePrimitive::TRIANGLES;
    buffer_quad.vertices =  { { {-1.0, -1.0, 0.0}, {0.0, 0.0} },
                              { { 1.0, -1.0, 0.0}, {1.0, 0.0} },
                              { { 1.0,  1.0, 0.0}, {1.0, 1.0} },

                              { {-1.0, -1.0, 0.0}, {0.0, 0.0} },
                              { { 1.0,  1.0, 0.0}, {1.0, 1.0} },
                              { {-1.0,  1.0, 0.0}, {0.0, 1.0} } };



    /* rasterizer with framebuffer size */
    Renderer rasterizer(480, 480);

    /* clear framebuffers */
    framebuffer.clear(Vec4(1, 1, 0, 1));
    rasterizer.framebuffer().clear(Vec4(0, 0, 0, 1));

    /* first render pass to draw triangle into custom framebuffer */
    rasterizer.draw(program_triangle, buffer_triangle, framebuffer);

    /* second render pass to blur previous framebuffer */
    rasterizer.draw(program_quad, buffer_quad);

    /* save framebuffer as .png */
    rasterizer.framebuffer().color().save("04_framebuffer.png");

    return EXIT_SUCCESS;
}
