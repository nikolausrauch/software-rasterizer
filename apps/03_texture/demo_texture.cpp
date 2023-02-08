#include <cstdlib>

#include <renderer.h>
#include <sampler.h>
#include <math/matrix4.h>

struct Vertex
{
    Vec3 pos;
    Vec2 uv;
};

struct Varying
{
    Vec4 position;
    Vec2 uv;

    VARYING( position, uv );
};

struct Uniforms
{
    Sampler<RGBA8> texture;
};

int main(int argc, char** argv)
{
    /*========== Setup Shader Program ========*/
    Program<Vertex, Varying, Uniforms> program;
    program.onVertex([](const Uniforms& uniform, const Vertex& in, Varying& out)
    {
        out.position = Vec4(in.pos, 1.0f);
        out.uv = in.uv;
    });

    program.onFragment([](const Uniforms& uniform, const Varying& in, Vec4& out)
    {
        Vec4 color = texture(uniform.texture, in.uv) / 255.0f;
        out = color;
    });


    /*========== Setup Buffer Data ========*/
    Buffer<Vertex> buffer;
    buffer.primitive = ePrimitive::TRIANGLES;
    buffer.vertices = { { {-0.5, -0.5, 0.5}, {0.0, 0.0} },
                        { { 0.5, -0.5, 0.5}, {1.0, 0.0} },
                        { { 0.0,  0.5, 0.5}, {0.5, 1.0} } };

    /* screen quad */
    Buffer<Vertex> buffer_quad;
    buffer_quad.primitive = ePrimitive::TRIANGLES;
    buffer_quad.vertices =  { { {-1.0, -1.0, 0.0}, {-1.0, -1.0} },
                              { { 1.0, -1.0, 0.0}, { 2.0, -1.0} },
                              { { 1.0,  1.0, 0.0}, { 2.0,  2.0} },

                              { {-1.0, -1.0, 0.0}, {-1.0, -1.0} },
                              { { 1.0,  1.0, 0.0}, { 2.0,  2.0} },
                              { {-1.0,  1.0, 0.0}, {-1.0,  2.0} } };


    /*========== Load Texture and set uniform ========*/
    Texture<RGBA8> texture;
    texture.load("assets/grid.png");
    program.uniforms().texture = texture;
    program.uniforms().texture.filter = eFilter::NEAREST;
    program.uniforms().texture.wrap = eWrap::CLAMP_EDGE;


    Renderer rasterizer(480, 480);

    rasterizer.framebuffer().clear(Vec4(0.2, 0.2, 0.2, 1));

    /* submit draw call */
    rasterizer.draw(program, buffer_quad);

    /* save framebuffer as .png */
    rasterizer.framebuffer().color().save("03_texture.png");

    return EXIT_SUCCESS;
}
