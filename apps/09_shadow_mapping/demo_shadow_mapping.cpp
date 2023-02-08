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
    Vec3 direction;

    Vec3 ambient;
    Vec3 color;
};



struct Varying
{
    Vec4 position;
    Vec2 uv;
    Vec3 normal;
    Vec3 world_position;
    Vec4 lighspace_position;

    VARYING( position, uv, normal, world_position, lighspace_position );
};

struct Uniforms
{
    Mat4 model;
    Mat4 view;
    Mat4 proj;

    Vec3 viewPos;
    Material material;
    Light light;
    Mat4 lightSpace;

    Sampler<Depth> shadow_map;
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

    Framebuffer<Depth> framebuffer_shadow(1028, 1028);
    Renderer::Options options_shadow{ {0, 0,
                    static_cast<float>(framebuffer_shadow.depth().width()),
                    static_cast<float>(framebuffer_shadow.depth().height())},
                                      true, false };

    /*========== Setup Shader Program (1. render pass - shadow mapp) ========*/
    Program<Vertex, Varying, Uniforms, Framebuffer<Depth>> program_shadow;
    program_shadow.onVertex([](const auto& uniform, const auto& in, auto& out)
    {
        out.position = uniform.lightSpace * uniform.model * Vec4(in.position, 1.0f);
    });

    program_shadow.onFragment([](const auto& uniform, const auto& in, auto& out)
    {

    });


    /*========== Setup Shader Program (2. render pass - lighting) ========*/
    Program<Vertex, Varying, Uniforms> program_light;
    program_light.onVertex([](const auto& uniform, const auto& in, auto& out)
    {
        auto word_pos = uniform.model * Vec4(in.position, 1.0f);
        out.position = uniform.proj * uniform.view * word_pos;
        out.world_position = Vec3(word_pos);
        out.normal = in.normal;
        out.uv = in.texcoord;
        out.lighspace_position = uniform.lightSpace * word_pos;
    });

    program_light.onFragment([](const auto& uniform, const auto& in, auto& out)
    {
        auto normal = normalize(in.normal);
        auto viewDir = normalize(uniform.viewPos - in.world_position);
        auto lightDir = normalize(-uniform.light.direction);

        Vec3 illuminance = uniform.light.ambient * uniform.material.ambient * uniform.material.diffuse;

        auto projCoords = Vec3(in.lighspace_position) / in.lighspace_position.w;
        projCoords = projCoords * 0.5 + Vec3(0.5, 0.5, 0.5);
        float currentDepth = projCoords.z;
        float adjBias = std::max(0.003 * (1.0 - dot(in.normal, uniform.light.direction)), 0.003 / 10.0);

        float closestDepth = texture(uniform.shadow_map, Vec2(projCoords));

        if(currentDepth - adjBias < closestDepth)
        {
            illuminance += uniform.light.color
                    * blinn_phong(lightDir, viewDir, normal,
                                  uniform.material.diffuse,
                                  uniform.material.specular,
                                  uniform.material.shininess);
        }

        out = Vec4(illuminance, 1.0);
    });




    /* load model */
    auto model = asset::loadObj<Mesh>("assets/low_poly_house/low_poly_house.obj");

    std::vector< BufferIndexed<Vertex, unsigned int> > buffers;
    std::vector< Material > materials;
    for(auto& mesh : model.meshes())
    {
        auto& buffer = buffers.emplace_back();
        buffer.vertices = mesh.vertices();
        buffer.indices = mesh.materialGroups().front().indices;

        auto& material = materials.emplace_back();
        material = mesh.material(0);
    }

    /* set uniforms Blinn Phong program */
    auto& uniforms_light = program_light.uniforms();
    uniforms_light.model = Mat4::scale(0.5, 0.5, 0.5);
    uniforms_light.proj = Mat4::perspective(radians(45.0f), 1280.0f/720.0f, 1.0, 15.0);
    uniforms_light.viewPos = Vec3{3.0, 3.5, 3.0};
    uniforms_light.view = Mat4::lookAt(uniforms_light.viewPos, Vec3{0, 0, 0});

    uniforms_light.light.direction = normalize(Vec3(1, -1, -1));
    uniforms_light.light.color = Vec3(1, 1, 1);
    uniforms_light.light.ambient = Vec3(0.2, 0.2, 0.2);
    uniforms_light.shadow_map = framebuffer_shadow.depth();

    /* set uniforms shadow program */
    auto& uniforms_shadow = program_shadow.uniforms();
    uniforms_shadow.model = uniforms_light.model;


    /*========== OpenGL/GLFW Viewer ========*/
    Window window("Software-Rasterizer Blinn Phong", 1280, 720);

    window.onDraw([&](Window& window, float dt)
    {
        rasterizer.framebuffer().clear(Vec4(0, 0, 0, 1));
        framebuffer_shadow.clear();

        uniforms_light.light.direction = Mat3::rotationY(-radians(dt*10.0f)) * uniforms_light.light.direction;
        uniforms_light.lightSpace = Mat4::ortho(-2.5, -2.5, 2.5, 2.5, 0, 10) * Mat4::lookAt(uniforms_light.light.direction * -3.0f, Vec3{0.5, 0, 0.5});
        uniforms_shadow.lightSpace = uniforms_light.lightSpace;

        TIME_MS(
        /* first render pass to create shadow map */
        for(unsigned int i = 0; i < buffers.size(); i++)
        {
            rasterizer.draw(program_shadow, buffers[i], framebuffer_shadow, options_shadow);
        }

        /* second render pass to determine light contribution */
        for(unsigned int i = 0; i < buffers.size(); i++)
        {
            uniforms_light.material = materials[i];
            rasterizer.draw(program_light, buffers[i]);
        }
        );

        window.swap(rasterizer.framebuffer());
    });

    window.onKeyboard([&](Window &window, int key, int mod, bool press)
    {
        if(key == GLFW_KEY_ENTER && press)
        {
            rasterizer.framebuffer().color().save("shadow_map_result.png");
            framebuffer_shadow.depth().save("shadow_map.png");
        }
    });

    window.run();

    return EXIT_SUCCESS;
}
