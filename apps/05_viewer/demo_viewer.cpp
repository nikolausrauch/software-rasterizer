#include <cstdlib>

#include <renderer.h>
#include <math/matrix4.h>

#include <gl_window.h>

#include <timing.h>


struct Vertex
{
    Vec3 pos;
    Vec3 color;
};

struct Varying
{
    Vec4 position;
    Vec3 color;

    VARYING( position, color );
};

struct Uniforms
{
    Mat4 model;
    Mat4 view;
    Mat4 proj;
};

int main(int argc, char** argv)
{
    Renderer rasterizer(1280, 720);

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
    uniforms.view = Mat4::translation(-Vec3{0, 0.0, 3.5});

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


    /*========== OpenGL/GLFW Viewer ========*/
    Window window("Software-Rasterizer Viewer", 1280, 720);

    window.onDraw([&](Window& window, float dt)
    {
        rasterizer.framebuffer().clear(Vec4(0, 0, 0, 1));

        static float time = 0.0;
        time += dt;

        uniforms.model = Mat4::rotationY(radians(time*20.0f)) * Mat4::rotationX(radians(45.0f)) * Mat4::scale(0.75, 0.75, 0.75);
        TIME_MS(rasterizer.draw(program, buffer_cube));

        window.swap(rasterizer.framebuffer());
    });

    window.run();

    return EXIT_SUCCESS;
}
