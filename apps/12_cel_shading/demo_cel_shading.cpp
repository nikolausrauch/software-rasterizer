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

    struct
    {
        Sampler<float> intensityMap;
        Sampler<RGBA8> diffuse;
    } material;

    Vec3 viewPos;
    Light light;

    bool celShading;
    bool renderEdge;
};


float linearizeDepth(float depth, const Vec2& nearFar)
{
    float z_n = 2.0 * depth - 1.0;
    float linearDepth = (2.0 * nearFar.x * nearFar.y) / (nearFar.y + nearFar.x - z_n * (nearFar.y - nearFar.x));
    return linearDepth;
}

void renderEdge(DefaultFramebuffer& fb)
{
    auto& color = fb.color();
    Sampler<float> depth = fb.depth();
    depth.filter = eFilter::NEAREST;

    Vec2 nearFar = {7.0f, 1.0f};
    Vec2 texSize = textureSize(depth);

    Vec2 step_u = Vec2(0.0, 1.0) / texSize;
    Vec2 step_r = Vec2(1.0, 0.0) / texSize;

    /* pixel distance used to find edges */
    float pixelWidth = 3.0f;
    step_u *= pixelWidth;
    step_r *= pixelWidth;

    float depth_x[3];
    float depth_y[3];

    for(int x = 0; x < texSize.x; x++)
    {
        for(int y = 0; y < texSize.y; y++)
        {
            Vec2 uv = (Vec2(x, y) + Vec2(0.5f, 0.5f)) / texSize;

            depth_x[1] = linearizeDepth(texture(depth, uv), nearFar);
            depth_y[1] = depth_x[1];

            depth_x[2] = linearizeDepth(texture(depth, uv + step_r), nearFar);
            depth_x[0] = linearizeDepth(texture(depth, uv - step_r), nearFar);

            depth_y[2] = linearizeDepth(texture(depth, uv + step_u), nearFar);
            depth_y[0] = linearizeDepth(texture(depth, uv - step_u), nearFar);

            /* hacky version of an edge detection */
            float grad_r = abs(depth_x[1] - depth_x[2]);
            float grad_l = abs(depth_x[1] - depth_x[0]);
            float x_grad = std::max(grad_r, grad_l);

            float grad_u = abs(depth_y[1] - depth_y[2]);
            float grad_d = abs(depth_y[1] - depth_y[0]);
            float y_grad = std::max(grad_u, grad_d);

            /* magnitude threshold to detect edge */
            float threshold = 0.075f;
            color(x, y) = length(Vec2(x_grad, y_grad)) > threshold ? RGBA8(0, 0, 0, 255) : color(x, y);
        }
    }
}

int main(int argc, char** argv)
{
    /* model */
    auto model = asset::loadObj<Mesh>("assets/bird/bird.obj");
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

    /* intensity buckets for cel shading (by u-texture coordinate 0-1) */
    Texture<float> intMap(4, 1);
    intMap(0, 0) = 0.1f;
    intMap(1, 0) = 0.2f;
    intMap(2, 0) = 0.4f;
    intMap(3, 0) = 0.8f;


    /*========== Setup Shader Program ========*/
    Program<Vertex, Varying, Uniforms> program;
    program.onVertex([](const Uniforms& uniform, const Vertex& in, Varying& out)
    {
        auto word_pos = uniform.model * Vec4(in.position, 1.0f);
        out.position = uniform.proj * uniform.view * word_pos;
        out.world_position = Vec3(word_pos);
        out.normal = Mat3(uniform.model) * in.normal;
        out.uv = in.texcoord;
    });

    program.onFragment([](const Uniforms& uniform, const Varying& in, Vec4& out)
    {
        auto normal = normalize(in.normal);
        auto viewDir = normalize(uniform.viewPos - in.world_position);
        auto lightDir = normalize(uniform.light.position - in.world_position);
        auto diffuse = Vec3(texture(uniform.material.diffuse, in.uv));

        /* Blinn-Phong */
        auto halfwayDir = normalize(lightDir + viewDir);
        float diff = std::max<float>(dot(normal, lightDir), 0.0);
        float spec = std::pow(std::max<float>(dot(normal, halfwayDir), 0.0), 128.0f);

        /* map illumination intensity to cel-shading buckets */
        diff = uniform.celShading ? texture(uniform.material.intensityMap, Vec2(diff, 0.5f)) : diff;
        spec = uniform.celShading ? texture(uniform.material.intensityMap, Vec2(spec, 0.5f)) : spec;

        Vec3 illuminance = uniform.light.ambient * diffuse;
        illuminance += (diff * diffuse) + (spec*Vec3(64, 64, 64));
        out = Vec4(illuminance / 255.0f, 1.0f);
    });


    /* set uniforms */
    auto& uniforms = program.uniforms();
    uniforms.proj = Mat4::perspective(radians(45.0f), 1280.0f/720.0f, 1.0, 7.0);
    uniforms.view = Mat4::translation(-Vec3{0, 0.0, 3.5});
    uniforms.viewPos = Vec3(0, 0.0, 3.5);
    uniforms.material.diffuse.filter = eFilter::LINEAR;
    uniforms.material.intensityMap = intMap;
    uniforms.material.intensityMap.filter = eFilter::NEAREST;
    uniforms.light = { .position = {3.0, 2.0, 3.0}, .ambient = {0.2f, 0.2f, 0.2f}, .color {1.0f, 1.0f, 1.0f} };
    uniforms.celShading = true;
    uniforms.renderEdge = true;


    /*========== OpenGL/GLFW Viewer ========*/
    Window window("Software-Rasterizer Cel Shading", 1280, 720);

    /* rasterizer */
    Renderer rasterizer(1280, 720);

    window.onDraw([&](Window& window, float dt)
    {
        rasterizer.framebuffer().clear(Vec4(1, 1, 1, 1));

        static float time = 0.0;
        time += dt;

        uniforms.model = Mat4::rotationY(-radians(time*20.0f));

        /* PASS 1: draw to framebuffer */
        for(unsigned int i = 0; i < buffers.size(); i++)
        {
            uniforms.material.diffuse = materials[i].map_diffuse;

            TIME_MS(rasterizer.draw(program, buffers[i]));
        }

        /* PASS 2: use depth buffer for edge detection */
        if(uniforms.renderEdge)
        {
            TIME_MS(renderEdge(rasterizer.framebuffer()));
        }

        window.swap(rasterizer.framebuffer());
    });


    window.onKeyboard([&](Window &window, int key, int mod, bool press)
    {
        if(key == GLFW_KEY_C && press)
        {
            uniforms.celShading = !uniforms.celShading;
        }

        if(key == GLFW_KEY_E && press)
        {
            uniforms.renderEdge = !uniforms.renderEdge;
        }
    });

    window.run();

    return EXIT_SUCCESS;
}
