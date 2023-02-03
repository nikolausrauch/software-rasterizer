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
    float shininess;
    Vec3 ambient;
    Vec3 diffuse;
    Vec3 specular;
};

/* Mesh definition (Vertex and Material --> see model.h for all possible members (names must match)) */
using Mesh = asset::Mesh<Vertex, Material>;

struct Light
{
    Vec3 position;

    Vec3 ambient;
    Vec3 color;
};



struct Varying
{
    Vec4 position;
    Vec2 uv;
    Vec3 normal;
    Vec3 world_position;

    VARYING( position, uv, normal, world_position );
};

struct Uniforms
{
    Mat4 model;
    Mat4 view;
    Mat4 proj;

    Vec3 viewPos;
    Material material;
    Light light;
};

Vec3 blinn_phong(const Vec3& lightDir, const Vec3& viewDir, const Vec3& normal, const Vec3& diffuse, const Vec3& specular, float shininess)
{
    auto halfwayDir = normalize(lightDir + viewDir);

    float diff = std::max<float>(dot(normal, lightDir), 0.0);
    float spec = std::pow(std::max<float>(dot(normal, halfwayDir), 0.0), shininess);

    return (diff * diffuse) + (spec * specular);
}

int main(int argc, char** argv)
{
    Renderer rasterizer(1280, 720);

    /*========== Setup Shader Program ========*/
    Program<Vertex, Varying, Uniforms> program;
    program.onVertex([](const Uniforms& uniform, const Vertex& in, Varying& out)
    {
        auto word_pos = uniform.model * Vec4(in.position, 1.0f);
        out.position = uniform.proj * uniform.view * word_pos;
        out.world_position = Vec3(word_pos);
        out.normal = in.normal;
        out.uv = in.texcoord;
    });

    program.onFragment([](const Uniforms& uniform, const Varying& in, Vec4& out)
    {
        auto normal = normalize(in.normal);
        auto viewDir = normalize(uniform.viewPos - in.world_position);
        auto lightDir = normalize(uniform.light.position - in.world_position);

        Vec3 illuminance = uniform.light.ambient * uniform.material.ambient * uniform.material.diffuse;
        illuminance += uniform.light.color
                * blinn_phong(lightDir, viewDir, normal, uniform.material.diffuse, uniform.material.specular, uniform.material.shininess);

        out = Vec4(illuminance, 1.0);
    });

    /* load model */
    auto model = asset::loadObj<Mesh>("assets/sphere/sphere.obj");
    auto& mesh = model.meshes().front();

    /* create draw buffer */
    BufferIndexed<Vertex, unsigned int> buffer;
    buffer.vertices = mesh.vertices();
    buffer.indices = mesh.materialGroups().front().indices;

    /* set uniforms */
    auto& uniforms = program.uniforms();
    uniforms.model = Mat4::identity();
    uniforms.proj = Mat4::perspective(radians(45.0f), 1280.0f/720.0f, 1.0, 7.0);
    uniforms.view = Mat4::translation(-Vec3{0, 0.0, 3.5});
    uniforms.viewPos = Vec3(0, 0.0, 3.5);

    uniforms.material = mesh.materialGroups().front().material;

    uniforms.light.position = Vec3(3, 2, 3);
    uniforms.light.color = Vec3(1, 1, 1);
    uniforms.light.ambient = Vec3(0.2, 0.2, 0.2);


    /*========== OpenGL/GLFW Viewer ========*/
    Window window("Software-Rasterizer Blinn Phong", 1280, 720);

    window.onDraw([&](Window& window, float dt)
    {
        rasterizer.framebuffer().clear(Vec4(0, 0, 0, 1));

        static float time = 0.0;
        time += dt;

        uniforms.light.position = Mat3::rotationY(-radians(dt*20.0f)) * uniforms.light.position;
        TIME_MS(rasterizer.draw(program, buffer));

        window.swap(rasterizer.framebuffer());
    });

    window.run();

    return EXIT_SUCCESS;
}
