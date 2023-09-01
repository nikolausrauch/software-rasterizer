#include <cstdlib>

#include <math/utility.h>

#include <timing.h>
#include <sampler.h>

#include <random>

// Taken from https://github.com/SaschaWillems/Vulkan-glTF-PBR/blob/master/data/shaders/genbrdflut.frag
// Based on http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
Vec2 hammersley(unsigned i, unsigned N)
{
    unsigned bits = (i << 16u) | (i >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    float rdi = static_cast<float>(bits) * 2.3283064365386963e-10;
    return Vec2(static_cast<float>(i) /static_cast<float>(N), rdi);
}

Vec3 sample_GGX(Vec2 Xi, float roughness, Vec3 N)
{
    float a = roughness * roughness;

    float phi = 2.0 * PI * Xi.x;
    float cos_theta = std::sqrt((1.0f - Xi.y) / (1.0f + (a * a - 1.0f) * Xi.y));
    float sin_theta = std::sqrt(1.0f - cos_theta * cos_theta);

    Vec3 H = {sin_theta * cos(phi), sin_theta * sin(phi), cos_theta};

    Vec3 U = abs(N.z) < 0.999 ? Vec3(0.0, 0.0, 1.0) : Vec3(1.0, 0.0, 0.0);
    Vec3 T_x = normalize(cross(U, N));
    Vec3 T_y = cross(N, T_x);
    return normalize(T_x * H.x + T_y * H.y + N * H.z);
}

float NDF_GGX(float nh, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float d = (nh*nh * (a2 - 1.0) + 1.0);
    return a2 / (PI * d * d);
}

float G_Smith(float nv, float nl, float roughness)
{
    float k = (roughness * roughness) / 2.0f;
    return (nv / (nv * (1.0f - k) + k)) * (nl / (nl * (1.0f - k) + k));
}

/* https://bruop.github.io/ibl/ */
Vec4 integrate_diffuse(const Vec3& N, const Sampler<RGBAF>& sampler_env, float level)
{
    Vec3 U = {0.0f, 1.0f, 0.0f};
    auto R = normalize(cross(U, N));
    U = normalize(cross(N, R));

    Vec4 irradiance = Vec4(0.0);
    float sample_count = 0.0f;

    float dPhi = PI_two / 360.0f;
    float dTheta = PI_half / 90.0f;

    for(float phi = 0.0; phi < PI_two; phi += dPhi)
    {
        for(float theta = 0.0; theta < PI_half; theta += dTheta)
        {
            Vec3 vec = {std::sin(theta) * std::cos(phi), std::cos(theta), std::sin(theta) * std::sin(phi)};
            auto vec_sample =  vec.x * R + vec.y * N + vec.z * U;

            irradiance += textureLod(sampler_env, equirectangularUV(vec_sample), level) * std::cos(theta) * std::sin(theta);

            sample_count += 1.0;
        }
    }

    return PI * irradiance / sample_count;
}

/* https://bruop.github.io/ibl/ */
Vec4 integrate_specular(Vec3 N, float roughness, const Sampler<RGBAF>& sampler_env)
{
    auto R = N;
    auto V = R;

    const unsigned int sample_count = 1024u;
    float weigth = 0.0;
    Vec4 color(0.0);
    for(unsigned int i = 0u; i < sample_count; i++)
    {
        auto Xi = hammersley(i, sample_count);
        auto H  = sample_GGX(Xi, roughness, N);
        auto L  = normalize(2.0 * dot(V, H) * H - V);

        auto nl = std::max(dot(N, L), 0.0f);
        auto nh = std::max(dot(N, H), 0.0f);
        auto hv = std::max(dot(H, V), 0.0f);
        if(nl > 0.0)
        {
            float D   = NDF_GGX(nh, roughness);
            float pdf = (D * nh / (4.0 * hv)) + 0.0001;

            float res = textureSize(sampler_env).x;
            float sa_texel  = 4.0f * PI / (6.0f * res * res);
            float sa_sample = 1.0f / (float(sample_count) * pdf + 0.0001);
            float level = roughness == 0.0f ? 0.0f : 0.5f * std::log2(sa_sample / sa_texel);

            color += textureLod(sampler_env, equirectangularUV(L), level) * nl;
            weigth += nl;
        }
    }

    return color / weigth;
}

Vec2 integrate_brdf(float nv, float roughness)
{
    Vec3 V = {std::sqrt(1.0f - nv*nv), 0.0f, nv};

    float A = 0.0f;
    float B = 0.0f;

    Vec3 N = {0.0, 0.0, 1.0};

    const unsigned int sample_count = 1024u;
    for(unsigned int i = 0u; i < sample_count; i++)
    {
        auto Xi = hammersley(i, sample_count);
        auto H  = sample_GGX(Xi, roughness, N);
        auto L  = normalize(2.0 * dot(V, H) * H - V);

        auto nl = std::max(L.z, 0.0f);
        auto nh = std::max(H.z, 0.0f);
        auto vh = std::max(dot(V, H), 0.0f);
        auto nv = std::max(dot(N, V), 0.0f);

        if(nl > 0.0)
        {
            float G = G_Smith(nv, nl, roughness);
            float G_Vis = (G * vh) / (nh * nv);
            float Fc = std::pow(1.0f - vh, 5.0f);

            A += (1.0f - Fc) * G_Vis;
            B += Fc * G_Vis;
        }
    }

    A /= static_cast<float>(sample_count);
    B /= static_cast<float>(sample_count);
    return Vec2(A, B);
}

void print_progress(int bar_width, float progress)
{
    int bar_progress = std::round(progress * bar_width);

    std::cout << "[";
    for(int i = 0; i < bar_progress; i++) { std::cout << "="; }
    std::cout << ">";
    for(int i = bar_progress; i < bar_width; i++) { std::cout << " "; }
    std::cout << "] " << static_cast<int>(progress * 100.0f) << " %\r";
    std::cout.flush();
}

int main(int argc, char** argv)
{   
    Texture<RGBAF> texture_env;
    if(!load_texture(texture_env, "assets/rural_asphalt_road_1k.hdr"))
    {
        std::cerr << "Couldn't load hdr image" << std::endl;
        return EXIT_FAILURE;
    }
    texture_env.generate_mipmaps();

    Sampler<RGBAF> sampler_env;
    sampler_env = texture_env;
    sampler_env.filter = eFilter::NEAREST_MIPMAP_LINEAR;

    /*---------- precompute irradiance map ----------*/
    Texture<RGBAF> texture_irradiance(96*2, 96);
    {
        std::cout << "[ irradiance texture " << texture_irradiance.width() << "x" << texture_irradiance.height() << " ]" << std::endl;

        for(int y = 0; y < texture_irradiance.height(); y++)
        {
            for(int x = 0; x < texture_irradiance.width(); x++)
            {
                auto uv = Vec2{ (x + 0.5f) / texture_irradiance.width(), (y + 0.5f) / texture_irradiance.height() };
                auto N = equirectangulatDir(uv * textureSize(sampler_env), textureSize(sampler_env));

                /*
                 * needs a large number of samples to avoid ringing artifacts
                 * sample from higher mipmap level (3.0) to avoid this at the cost of accuracy
                */
                texture_irradiance(x, y) = integrate_diffuse(N, sampler_env, 3.0f);
            }

            print_progress(40, y / static_cast<float>(texture_irradiance.height()));
        }

        if(!save_texture(texture_irradiance, "precomputed_irradiance.hdr"))
        {
            std::cerr << "Couldn't save precomputed_irradiance.hd!" << std::endl;
        }
    }


    /*---------- precompute radiance for specular reflections ----------*/
    Texture<RGBAF> texture_radiance(128*6, 128*3);
    texture_radiance.generate_mipmaps();
    unsigned int max_levels = texture_radiance.num_mipmaps();
    {
        std::cout << "[ radiance texture " << texture_radiance.width() << "x" << texture_radiance.height() << " ]" << std::endl;

        for(unsigned int mip = 0; mip < max_levels; mip++)
        {
            float roughness = static_cast<float>(mip) / static_cast<float>(max_levels - 1);
            auto& texture_level = texture_radiance.mipmaps()[mip];

            std::cout << "[ level =  " << mip << ", roughness = " << roughness << ", size = " << texture_level.width() << "x" << texture_level.height() << " ]" << std::endl;

            for(int y = 0; y < texture_level.height(); y++)
            {
                for(int x = 0; x < texture_level.width(); x++)
                {
                    auto uv = Vec2{ (x + 0.5f) / texture_level.width(), (y + 0.5f) / texture_level.height() };
                    auto N = equirectangulatDir(uv * textureSize(sampler_env), textureSize(sampler_env));

                    texture_level(x, y) = integrate_specular(N, roughness, sampler_env);
                }

                print_progress(40, y / static_cast<float>(texture_level.height()));
            }

        }

        if(!save_mipmaps(texture_radiance, "./", "precomputed_radiance.hdr"))
        {
            std::cerr << "Couldn't save precomputed_radiance.hd mipmap chain!" << std::endl;
        }
    }


    /*---------- precompute brdf ----------*/
    Texture<RGBAF> texture_brdf(128, 128);
    {
        std::cout << "[ brdf texture " << texture_brdf.width() << "x" << texture_brdf.height() << " ]" << std::endl;

        for(int y = 0; y < texture_brdf.height(); y++)
        {
            for(int x = 0; x < texture_brdf.width(); x++)
            {
                auto uv = Vec2{ (x + 0.5f) / texture_brdf.width(), (y + 0.5f) / texture_brdf.height() };
                texture_brdf(x, y) = Vec4(integrate_brdf(uv.x, uv.y), 0.0f, 1.0f);
            }

            print_progress(40, y / static_cast<float>(texture_brdf.height()));
        }

        if(!save_texture(texture_brdf, "precomputed_brdf.hdr"))
        {
            std::cerr << "Couldn't save precomputed_brdf.hd!" << std::endl;
        }
    }

    return EXIT_SUCCESS;
}
