#include <cstdlib>

#include <renderer.h>
#include <math/matrix4.h>

#include <gl_window.h>

#include <timing.h>
#include <sampler.h>
#include <model.h>
#include <objload.h>


struct Vertex
{
    Vec3 position;
    Vec3 normal;
    Vec2 texcoord;
};

struct Material
{
    Texture<RGBA8> map_diffuse;
};

/* Mesh definition (Vertex and Material --> see model.h for all possible members) */
using Mesh = asset::Mesh<Vertex, Material>;


struct Varying
{
    Vec4 position;
    Vec2 uv;

    VARYING( position, uv );
};

struct Uniforms
{
    Mat4 model;
    Mat4 view;
    Mat4 proj;

    struct
    {
        Sampler<RGBA8> diffuse;
    } material;
};


int main(int argc, char** argv)
{
    Renderer rasterizer(1280, 720);

    /*========== Setup Shader Program ========*/
    Program<Vertex, Varying, Uniforms> program;
    program.onVertex([](const Uniforms& uniform, const Vertex& in, Varying& out)
    {
        out.position = uniform.proj * uniform.view * uniform.model * Vec4(in.position, 1.0f);
        out.uv = in.texcoord;
    });

    program.onFragment([](const Uniforms& uniform, const Varying& in, Vec4& out)
    {
        out = texture(uniform.material.diffuse, in.uv) / 255.0f;
    });

    /* model */
    auto model = asset::loadObj<Mesh>("assets/sad_toaster/sad_toaster.obj");
    auto& mesh = model.meshes().front();
    BufferIndexed<Vertex, unsigned int> buffer;
    buffer.vertices = mesh.vertices();
    buffer.indices = mesh.materialGroups().front().indices;


    /* set uniforms */
    auto& uniforms = program.uniforms();
    uniforms.proj = Mat4::perspective(radians(45.0f), 1280.0f/720.0f, 1.0, 7.0);
    uniforms.view = Mat4::translation(-Vec3{0, 0.0, 3.5});
    uniforms.material.diffuse = mesh.materialGroups().front().material.map_diffuse;
    uniforms.material.diffuse.filter = eFilter::LINEAR;

    /*========== OpenGL/GLFW Viewer ========*/
    Window window("Software-Rasterizer Model", 1280, 720);

    window.onDraw([&](Window& window, float dt)
    {
        rasterizer.framebuffer().clear(Vec4(0, 0, 0, 1));

        static float time = 0.0;
        time += dt;

        uniforms.model = Mat4::translation(Vec4{0, -0.75, 0}) * Mat4::rotationY(-radians(time*20.0f));
        TIME_MS(rasterizer.draw(program, buffer));

        window.swap(rasterizer.framebuffer());
    });


    window.onKeyboard([&](Window &window, int key, int mod, bool press)
    {
        if(key == GLFW_KEY_W && press)
        {
            rasterizer.options().wireframe = !rasterizer.options().wireframe;
        }
    });

    window.run();

    return EXIT_SUCCESS;
}
