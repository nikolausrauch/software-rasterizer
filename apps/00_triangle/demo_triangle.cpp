#include <cstdlib>

#include <renderer.h>
#include <math/matrix4.h>

/* vertex data -> input to draw call (via Buffer) */
struct Vertex
{
    Vec3 pos;
    Vec3 color;
};

/*
 * Output of vertex stage, Input to fragment stage
 * -> position is mandatory
 * -> members to interpolate are declared by VARYING and VAL Macros (member need scalar multiplication, and addition)
*/
struct Varying
{
    Vec4 position;
    Vec3 color;

    VARYING(position, color);
};

/* uniform struct accessable from both "shaders" */
struct Uniforms
{

};

int main(int argc, char** argv)
{
    /*========== Setup Shader Program ========*/
    Program<Vertex, Varying, Uniforms> program;
    program.onVertex([](const Uniforms& uniform, const Vertex& in, Varying& out)
    {
        out.position = Vec4(in.pos, 1.0f);
        out.color = in.color;
    });

    program.onFragment([](const Uniforms& uniform, const Varying& in, Vec4& out)
    {
        out = Vec4(in.color, 1.0f);
    });

    /*========== Setup Buffer Data ========*/
    Buffer<Vertex> buffer;
    buffer.primitive = ePrimitive::TRIANGLES;
    buffer.vertices = { { {-0.5, -0.5, 0.5}, {1.0, 0.0, 0.0} },
                        { { 0.5, -0.5, 0.5}, {0.0, 1.0, 0.0} },
                        { { 0.0,  0.5, 0.5}, {0.0, 0.0, 1.0} } };



    /* rasterizer with framebuffer size */
    Renderer rasterizer(480, 480);

    /* clear framebuffer */
    rasterizer.framebuffer().clear(Vec4(0, 0, 0, 1));

    /* submit draw call */
    rasterizer.draw(program, buffer);

    /* save framebuffer as .png */
    rasterizer.framebuffer().color().save("00_triangle.png");

    return EXIT_SUCCESS;
}
