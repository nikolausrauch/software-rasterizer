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
    Mat4 model;
    Mat4 view;
    Mat4 proj;
};

int main(int argc, char** argv)
{
    /*========== Setup Shader Program ========*/
    Program<Vertex, Varying, Uniforms> program;
    program.onVertex([](const Uniforms& uniform, const Vertex& in, Varying& out)
    {
        out.position = uniform.proj * uniform.view * uniform.model * Vec4(in.pos, 1.0f);
        out.color = in.color;
    });

    program.onFragment([](const Uniforms& uniform, const Varying& in, Vec4& out)
    {
        out = Vec4(in.color, 1.0);
    });

    /* set uniforms */
    auto& uniforms = program.uniforms();
    uniforms.proj = Mat4::perspective(radians(45.0f), 1280.0f/720.0f, 1.0, 7.0);
    uniforms.view = Mat4::translation(-Vec3{0, 0.25, 3.5}) * Mat4::rotationX(radians(10.0f));


    /*========== Setup Buffer Data ========*/
    /* plane */
    BufferIndexed<Vertex, unsigned int> buffer_plane;
    buffer_plane.primitive = ePrimitive::TRIANGLES;
    buffer_plane.vertices =
    {
        {{-1.0,   0.0, -1.0},   {1.0, 0.0, 0.0} },
        {{-1.0,   0.0,  1.0},   {1.0, 1.0, 1.0} },
        {{ 1.0,   0.0,  1.0},   {0.0, 0.0, 1.0} },
        {{ 1.0,   0.0, -1.0},   {0.0, 1.0, 0.0} }
    };
    buffer_plane.indices = { 0, 1, 2,     2, 3, 0 };

    /* cube */
    BufferIndexed<Vertex, unsigned int> buffer_cube;
    buffer_cube.primitive = ePrimitive::TRIANGLES;
    buffer_cube.vertices =
    {
        {{-1.0, -1.0, 1.0}, {1.0, 0.0, 0.0}},
        {{-1.0,  1.0, 1.0}, {0.0, 1.0, 0.0}},
        {{ 1.0,  1.0, 1.0}, {0.0, 0.0, 1.0}},
        {{ 1.0, -1.0, 1.0}, {1.0, 0.0, 1.0}},

        {{-1.0, -1.0, -1.0}, {1.0, 0.0, 0.0}},
        {{-1.0,  1.0, -1.0}, {0.0, 1.0, 0.0}},
        {{ 1.0,  1.0, -1.0}, {0.0, 0.0, 1.0}},
        {{ 1.0, -1.0, -1.0}, {1.0, 0.0, 1.0}}
    };
    buffer_cube.indices =
    {
        2, 1, 0,
        0, 3, 2,

        4, 5, 6,
        6, 7, 4,

        0, 1, 5,
        5, 4, 0,

        6, 2, 3,
        3, 7, 6,

        6, 5, 1,
        1, 2, 6,

        0, 4, 7,
        7, 3, 0
    };


    /* rasterizer with framebuffer size */
    Renderer rasterizer(1280, 720);

    /* clear framebuffer */
    rasterizer.framebuffer().clear(Vec4(0, 0, 0, 1));

    /* submit draw call */
    uniforms.model = Mat4::translation({-0.5, 0.2, 1.5}) * Mat4::scale(0.2, 0.2, 0.2);
    rasterizer.draw(program, buffer_cube);

    uniforms.model = Mat4::translation({0.5, 0.2, 1.0}) * Mat4::scale(0.2, 0.2, 0.2);
    rasterizer.draw(program, buffer_cube);

    uniforms.model = Mat4::translation({-0.4, 0.2, -0.3}) * Mat4::scale(0.2, 0.2, 0.2);
    rasterizer.draw(program, buffer_cube);

    uniforms.model = Mat4::scale(3.0, 2.0, 3.0);
    rasterizer.draw(program, buffer_plane);

    /* save framebuffer as .png */
    rasterizer.framebuffer().color().save("02_color_buffer.png");
    rasterizer.framebuffer().depth().save("02_depth_buffer.png");

    return EXIT_SUCCESS;
}
