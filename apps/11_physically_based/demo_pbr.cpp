#include <cstdlib>

#include <renderer.h>
#include <math/matrix4.h>

#include <gl_window.h>

#include <timing.h>
#include <sampler.h>
#include <model.h>
#include <objload.h>

#include <random>


struct Vertex
{
    Vec3 position;
    Vec3 normal;
    Vec2 texcoord;
    Vec4 tangent;
};

struct Material
{
    Texture<RGBA8> map_albedo;
    Texture<RGBA8> map_metallic_roughness;
    Texture<RGBA8> map_normal;
};

/* Mesh definition (Vertex and Material --> see model.h for all possible members) */
using Mesh = asset::Mesh<Vertex, Material>;

struct Varying
{
    Vec4 position;
    Vec3 world_position;
    Vec2 uv;
    Mat3 TBN;

    VARYING( position, world_position, uv, TBN );
};

struct Uniforms
{
    Mat4 model;
    Mat4 view;
    Mat4 proj;

    Vec3 viewPos;

    struct
    {
        Sampler<RGBA8> albedo;
        Sampler<RGBA8> metallic_roughness;
        Sampler<RGBA8> normal;
    } material;

    Sampler<RGBAF> irradiance;
    Sampler<RGBAF> prefilter_radiance;
    Sampler<RGBAF> brdf;
};

int main(int argc, char** argv)
{
    /* model */
    auto model = asset::loadObj<Mesh>("assets/camera/camera.obj");
    //auto model = asset::loadObj<Mesh>("assets/drakefire/drakefire.obj");
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

    /* load precomputed irradiance, radiance and brdf maps (see pbr_precompute.cpp) */
    Texture<RGBAF> texture_irradiance;
    Texture<RGBAF> texture_radiance;
    Texture<RGBAF> texture_brdf;
    load_texture(texture_irradiance, "assets/pbr/precomputed_irradiance.hdr");
    load_mipmaps(texture_radiance, "assets/pbr/", "precomputed_radiance.hdr");
    load_texture(texture_brdf, "assets/pbr/precomputed_brdf.hdr");


    /*========== Setup Shader Program ========*/
    Program<Vertex, Varying, Uniforms> program;
    program.onVertex([](const Uniforms& uniform, const Vertex& in, Varying& out)
    {
        out.world_position = Vec3(uniform.model * Vec4(in.position, 1.0f));
        out.position = uniform.proj * uniform.view * uniform.model * Vec4(in.position, 1.0f);
        out.uv = in.texcoord;

        auto model = Mat3(uniform.model);
        auto tangent = Vec3(in.tangent);
        auto bitangent = cross(in.normal, tangent) * in.tangent.w;
        out.TBN = Mat3( model * tangent, model * bitangent, model * in.normal);
    });

    program.onFragment([](const Uniforms& uniform, const Varying& in, Vec4& out)
    {
        auto n = normalize(in.TBN * normalize( Vec3(texture(uniform.material.normal, in.uv) / 255.0f) * 2.0f - 1.0f ) );
        Vec3 v = normalize(uniform.viewPos - in.world_position);
        float nv = std::max(dot(n, v), 0.0f);
        Vec3 r = reflect(-v, n);

        /* material properties */
        Vec3 albedo = Vec3(texture(uniform.material.albedo, in.uv) / 255.0f);
        auto metal_rough = texture(uniform.material.metallic_roughness, in.uv) / 255.0f;
        float metallic = metal_rough.z;
        float roughness = metal_rough.y;

        /* determine metallic / diffuse color values */
        const Vec3 DIELECTRIC_F0 = {0.04f, 0.04f, 0.04f};
        Vec3 f0 = mix(DIELECTRIC_F0, albedo, metallic);
        albedo = albedo * (1.0 - DIELECTRIC_F0.x) * (1.0 - metallic);

        /* retrieve irradiance and radiance from precomputed maps */
        Vec3 radiance = Vec3(textureLod(uniform.prefilter_radiance, equirectangularUV(r), roughness * uniform.prefilter_radiance.m_texture->num_mipmaps()));
        Vec3 irradiance = Vec3(texture(uniform.irradiance, equirectangularUV(n)));
        auto env_brdf  = Vec2(texture(uniform.brdf, Vec2(nv, roughness)));

        /* https://www.jcgt.org/published/0008/01/03/paper.pdf */
        Vec3 Fr = max(Vec3(1.0 - roughness), f0) - f0;
        Vec3 k_S = f0 + Fr * pow(1.0 - nv, 5.0);
        auto fss_ess = (k_S * env_brdf.x + Vec3(env_brdf.y));

        Vec3 illuminance = (fss_ess * radiance + albedo * irradiance);

        out = Vec4(illuminance, 1.0f);
    });

    /* set uniforms */
    auto& uniforms = program.uniforms();
    uniforms.proj = Mat4::perspective(radians(45.0f), 1280.0f/720.0f, 1.0, 7.0);
    uniforms.view = Mat4::translation(-Vec3{0, 0.0, 3.5});
    uniforms.viewPos = Vec3{0, 0.0, 3.5};

    uniforms.material.albedo.filter = eFilter::NEAREST_MIPMAP_LINEAR;
    uniforms.material.metallic_roughness.filter = eFilter::NEAREST_MIPMAP_LINEAR;
    uniforms.material.normal.filter = eFilter::NEAREST_MIPMAP_LINEAR;

    uniforms.irradiance = texture_irradiance;
    uniforms.irradiance.wrap = eWrap::CLAMP_EDGE;
    uniforms.prefilter_radiance = texture_radiance;
    uniforms.prefilter_radiance.wrap = eWrap::CLAMP_EDGE;
    uniforms.brdf = texture_brdf;
    uniforms.brdf.wrap = eWrap::CLAMP_EDGE;


    /* rasterizer */
    unsigned int fbwidth = 1280; unsigned fbheight = 720;
    Renderer rasterizer(fbwidth, fbheight);

    /*========== OpenGL/GLFW Viewer ========*/
    Window window("Physically-Based-Rendering", 1280, 720, fbwidth, fbheight);

    window.onDraw([&](Window& window, float dt)
    {
        rasterizer.framebuffer().clear(Vec4(0, 0, 0, 1));

        static float time = 0.0;
        time += dt;

        uniforms.model = Mat4::translation(Vec3{0, -0.25, 0}) * Mat4::scale(1.4, 1.4, 1.4) * Mat4::rotationY(-radians(160.0f + time*20.0f)) * Mat4::rotationX(radians(10.0f));

        for(unsigned int i = 0; i < buffers.size(); i++)
        {
            uniforms.material.albedo = materials[i].map_albedo;
            uniforms.material.metallic_roughness = materials[i].map_metallic_roughness;
            uniforms.material.normal = materials[i].map_normal;

            TIME_MS(rasterizer.draw(program, buffers[i]));
        }

        window.swap(rasterizer.framebuffer());
    });

    window.run();

    return EXIT_SUCCESS;
}
