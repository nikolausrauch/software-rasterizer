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
    Vec4 tangent;
};

struct Material
{
    float shininess;
    Vec3 ambient;
    Vec3 diffuse;
    Vec3 specular;

    Texture<RGBA8> map_diffuse;
    Texture<RGBA8> map_bump;
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
    Vec3 view_tangent;
    Vec3 light_tangent;

    VARYING( position, uv, normal, world_position, view_tangent, light_tangent );
};

struct Uniforms
{
    Mat4 model;
    Mat4 view;
    Mat4 proj;

    Vec3 viewPos;
    Light light;

    bool use_bump_mapping;

    struct
    {
        float shininess;
        Vec3 ambient;
        Vec3 diffuse;
        Vec3 specular;

        Sampler<RGBA8> map_diffuse;
        Sampler<RGBA8> map_bump;
    } material;
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
        out.world_position = Vec3(word_pos);

        out.position = uniform.proj * uniform.view * word_pos;
        out.normal = Mat3(uniform.model) * in.normal;
        out.uv = in.texcoord;

        /* transform viewdirection and lightdirection to tangent space */
        if(uniform.use_bump_mapping)
        {
            auto tangent = Vec3(in.tangent);
            auto bitangent = cross(in.normal, tangent) * in.tangent.w;
            auto v = uniform.viewPos - out.world_position;
            auto l = uniform.light.position - out.world_position;
            out.view_tangent = normalize(Vec3(dot(tangent, v), dot(bitangent, v), dot(in.normal, v)));
            out.light_tangent = normalize(Vec3(dot(tangent, l), dot(bitangent, l), dot(in.normal, l)));
        }
    });

    program.onFragment([](const Uniforms& uniform, const Varying& in, Vec4& out)
    {
        auto normal = normalize(in.normal);
        auto viewDir = normalize(uniform.viewPos - in.world_position);
        auto lightDir = normalize(uniform.light.position - in.world_position);

        if(uniform.use_bump_mapping)
        {
            /* retrieve normal vector and compute blinn phong in tangent space */
            normal = normalize( Vec3(texture(uniform.material.map_bump, in.uv) / 255.0f) * 2.0 - Vec3(1.0, 1.0, 1.0) );
            lightDir = in.light_tangent;
            viewDir = in.view_tangent;
        }

        Vec3 illuminance = uniform.light.ambient * uniform.material.ambient * uniform.material.diffuse;
        illuminance += uniform.light.color
                * blinn_phong(lightDir, viewDir, normal,
                              texture(uniform.material.map_diffuse, in.uv) / 255.0f,
                              uniform.material.specular,
                              uniform.material.shininess);

        out = Vec4(illuminance, 1.0);
    });


    /* load model */
    auto model = asset::loadObj<Mesh>("assets/cartridge/cartridge.obj");
    auto& mesh = model.meshes().front();

    /* create draw buffer */
    BufferIndexed<Vertex, unsigned int> buffer;
    buffer.vertices = mesh.vertices();
    buffer.indices = mesh.materialGroups().front().indices;

    /* set uniforms */
    auto& uniforms = program.uniforms();
    uniforms.model = Mat4::identity();
    uniforms.proj = Mat4::perspective(radians(45.0f), 1280.0f/720.0f, 1.0, 7.0);
    uniforms.viewPos = Vec3{3.0, 3.5, 3.0};
    uniforms.view = Mat4::lookAt(uniforms.viewPos, Vec3{0, 0, 0});
    uniforms.use_bump_mapping = true;

    auto& material = mesh.materialGroups().front().material;
    uniforms.material.shininess = material.shininess;
    uniforms.material.ambient = material.ambient;
    uniforms.material.diffuse = material.diffuse;
    uniforms.material.specular = material.specular;
    uniforms.material.map_diffuse = material.map_diffuse;
    uniforms.material.map_bump = material.map_bump;


    uniforms.light.position = Vec3(3, 2, 3);
    uniforms.light.color = Vec3(1, 1, 1);
    uniforms.light.ambient = Vec3(0.2, 0.2, 0.2);


    /*========== OpenGL/GLFW Viewer ========*/
    Window window("Software-Rasterizer Normal Mapping", 1280, 720);

    window.onDraw([&](Window& window, float dt)
    {
        rasterizer.framebuffer().clear(Vec4(0, 0, 0, 1));

        uniforms.light.position = Mat3::rotationY(-radians(dt*30.0f)) * uniforms.light.position;
        TIME_MS(rasterizer.draw(program, buffer));

        window.swap(rasterizer.framebuffer());
    });

    window.onKeyboard([&](Window &window, int key, int mod, bool press)
    {
        if(key == GLFW_KEY_SPACE && press)
        {
            uniforms.use_bump_mapping = !uniforms.use_bump_mapping;
        }

        if(key == GLFW_KEY_ENTER && press)
        {
            rasterizer.framebuffer().color().save("normal_mapping.png");
        }
    });

    window.run();

    return EXIT_SUCCESS;
}
