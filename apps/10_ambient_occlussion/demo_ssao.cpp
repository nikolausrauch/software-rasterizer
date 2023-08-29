#include <cstdlib>

#include <renderer.h>
#include <math/matrix4.h>

#include <gl_window.h>

#include <timing.h>
#include <sampler.h>
#include <model.h>
#include <objload.h>

#include <random>

namespace model
{

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
    Vec4 view_position;
    Vec3 normal;
    Vec2 uv;

    VARYING( position, view_position, normal, uv );
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
    Sampler<Vec4> view_position;
    Sampler<Vec3> normal;
    Sampler<RGBA8> diffuse;

    Mat4 proj;
    Sampler<Vec3> random_dir;
    std::vector<Vec3> samples;
    float radius;
    float bias;
    bool ambient_occlussion;
};

}

std::vector<Vec3> sample_kernel(unsigned int size)
{
    std::uniform_real_distribution<float> rand(0.0, 1.0);
    std::default_random_engine engine;

    std::vector<Vec3> dirs(size);
    for(unsigned int i = 0; i < size; i++)
    {
        dirs[i] = normalize(Vec3{ -1.0f + 2.0f * rand(engine), -1.0f + 2.0f * rand(engine), rand(engine) });
        dirs[i] *= rand(engine) * std::lerp(0.1f, 1.0f, std::pow(static_cast<float>(i) / size, 2.0f));
    }

    return dirs;
}

std::vector<Vec3> random_dir(unsigned int width, unsigned int height)
{
    std::uniform_real_distribution<float> rand(0.0, 1.0);
    std::default_random_engine engine;

    unsigned int size = width*height;
    std::vector<Vec3> normals(size);
    for(unsigned int i = 0; i < size; i++)
    {
        normals[i] = normalize( Vec3{-1.0f + 2.0f * rand(engine), -1.0f + 2.0f * rand(engine), 0.0} );
    }

    return normals;
}


template<typename T>
T smoothstep(const T edge_0, const T edge_1, const T x)
{
    auto t = std::clamp((x - edge_0) / (edge_1 - edge_0), 0.0f, 1.0f);
    return t * t * (3.0 - 2.0 * t);
}

int main(int argc, char** argv)
{
    /* model */
    auto model = asset::loadObj<model::Mesh>("assets/firered_room/room.obj");
    auto& mesh = model.meshes().front();
    BufferIndexed<model::Vertex, unsigned int> buffer_model;
    buffer_model.vertices = mesh.vertices();
    buffer_model.indices = mesh.materialGroups().front().indices;

    /* screen quad */
    Buffer<quad::Vertex> buffer_quad;
    buffer_quad.primitive = ePrimitive::TRIANGLES;
    buffer_quad.vertices =  { { {-1.0, -1.0, 0.0}, {0.0, 0.0} },
                              { { 1.0, -1.0, 0.0}, {1.0, 0.0} },
                              { { 1.0,  1.0, 0.0}, {1.0, 1.0} },

                              { {-1.0, -1.0, 0.0}, {0.0, 0.0} },
                              { { 1.0,  1.0, 0.0}, {1.0, 1.0} },
                              { {-1.0,  1.0, 0.0}, {0.0, 1.0} } };


    unsigned int fbwidth = 1080; unsigned fbheight = 720;

    Renderer rasterizer(fbwidth, fbheight);

    Framebuffer<Vec4, Vec3, RGBA8, Depth> g_buffer(fbwidth, fbheight);

    /*========== Setup Shader Program (1. geometry pass) ========*/
    Program<model::Vertex, model::Varying, model::Uniforms, decltype(g_buffer)> program_geometry;
    program_geometry.onVertex([](const auto& uniform, const auto& in, auto& out)
    {
        out.view_position = uniform.view * uniform.model * Vec4(in.position, 1.0f);
        out.position = uniform.proj * out.view_position;
        out.normal = Mat3(uniform.view * uniform.model) * in.normal;
        out.uv = in.texcoord;
    });

    program_geometry.onFragment([](const auto& uniform, const auto& in, auto& out)
    {
        auto& [position, normal, diffuse, depth] = out;
        position = in.view_position;
        normal = normalize(in.normal);
        diffuse = texture(uniform.material.diffuse, in.uv);
    });

    /* set uniforms */
    auto& uniforms_gpass = program_geometry.uniforms();
    uniforms_gpass.proj = Mat4::perspective(radians(45.0f), 1280.0f/720.0f, 1.0, 16.0);
    uniforms_gpass.view = Mat4::lookAt({6, 5, 7}, Vec3{0, 0, 0});
    uniforms_gpass.model = Mat4::translation(Vec3{0, 0, 0});
    uniforms_gpass.material.diffuse = mesh.materialGroups().front().material.map_diffuse;
    uniforms_gpass.material.diffuse.filter = eFilter::NEAREST;


    /*========== Setup Shader Program (2. light and occlussion pass) ========*/
    Program<quad::Vertex, quad::Varying, quad::Uniforms> program_light;
    program_light.onVertex([](const auto& uniform, const auto& in, auto& out)
    {
        out.position = Vec4(in.pos, 1.0f);
        out.uv = in.uv;
    });

    program_light.onFragment([](const auto& uniform, const auto& in, auto& out)
    {
        auto frag_pos = Vec3(texture(uniform.view_position, in.uv));
        auto normal = texture(uniform.normal, in.uv);

        if(length(normal) <= 1e-8) return;

        float occlusion = 0.0;
        if(uniform.ambient_occlussion)
        {
            normal = normalize(normal);
            auto rand_dir = normalize(texture(uniform.random_dir, in.uv));

            auto tangent = normalize( rand_dir - normal * dot(rand_dir, normal) );
            auto bitangent = cross(normal, tangent);
            Mat3 TBN = Mat3(tangent, bitangent, normal);

            for(unsigned int i = 0; i < uniform.samples.size(); i++)
            {
                auto sample_pos = TBN * uniform.samples[i];
                sample_pos = frag_pos + sample_pos * uniform.radius;

                auto offset = Vec4(sample_pos, 1.0);
                offset = uniform.proj * offset;
                offset /= offset.w;
                offset = offset * 0.5 + Vec4(0.5, 0.5, 0.5, 0.5);

                float sample_depth = texture(uniform.view_position, Vec2(offset)).z;
                float range_check = smoothstep(0.0f, 1.0f, uniform.radius / abs(frag_pos.z - sample_depth));
                occlusion += (sample_depth >= sample_pos.z + uniform.bias ? 1.0 : 0.0) * range_check;
            }
        }
        occlusion = 1.0 - (occlusion / uniform.samples.size());

        out = Vec4(occlusion, occlusion, occlusion, 1.0) * texture(uniform.diffuse, in.uv) / 255.0f;
    });

    /* texture containing random direction vectors to check occlussion */
    Texture<Vec3> normal_noise(4, 4);
    normal_noise.data(random_dir(4, 4));

    auto& uniform_light = program_light.uniforms();
    uniform_light.view_position = g_buffer.target<0>();
    uniform_light.normal = g_buffer.target<1>();
    uniform_light.diffuse = g_buffer.target<2>();
    uniform_light.diffuse.filter = eFilter::LINEAR;
    uniform_light.random_dir = normal_noise;
    uniform_light.random_dir.filter = eFilter::LINEAR;
    uniform_light.random_dir.wrap = eWrap::REPEAT;
    uniform_light.samples = sample_kernel(32);
    uniform_light.proj = uniforms_gpass.proj;
    uniform_light.radius = 0.25;
    uniform_light.bias = 0.0001;
    uniform_light.ambient_occlussion = true;


    /*========== OpenGL/GLFW Viewer ========*/
    Window window("Software-Rasterizer Viewer", 1280, 720, fbwidth, fbheight);

    window.onDraw([&](Window& window, float dt)
    {
        rasterizer.framebuffer().clear(Vec4(0, 0, 0, 1));
        g_buffer.clear( Vec4(0, 0, 0, 0) );

        TIME_MS(rasterizer.draw(program_geometry, buffer_model, g_buffer));

        TIME_MS(rasterizer.draw(program_light, buffer_quad));

        window.swap(rasterizer.framebuffer());
    });

    window.onKeyboard([&](Window& window, int key, int mod, bool press)
    {
        if(key == GLFW_KEY_SPACE && press)
        {
            uniform_light.ambient_occlussion = !uniform_light.ambient_occlussion;
        }
    });

    window.run();

    return EXIT_SUCCESS;
}
