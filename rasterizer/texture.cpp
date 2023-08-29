#include "texture.h"

#include <stb_image/stb_image_write.h>
#include <stb_image/stb_image.h>

#include <cstring>
#include <algorithm>

/*********************************************
 *                                           *
 *             RGBA Textures (byte)          *
 *                                           *
 *********************************************/

template<>
bool save_texture(const Texture<RGBA8>& texture, const std::string& filepath)
{
    static_assert(sizeof(RGBA8) == 4*sizeof(unsigned char), "RGBA8 is not tightly packed -- needs special handling to allow stbi to write image file!");

    stbi_flip_vertically_on_write(true);
    return stbi_write_png(filepath.c_str(), texture.width(), texture.height(), 4, texture.ptr(), texture.width() * 4);
}

template<>
bool save_texture(const Texture<Depth>& texture, const std::string& filepath)
{   
    std::vector<RGBA8> color(texture.width() * texture.height());
    for(unsigned int i = 0; i < color.size(); i++)
    {
        std::uint8_t value = 255.0f * std::clamp<float>(texture.data()[i], 0.0f, 1.0f);
        color[i] = RGBA8(value, value, value, 255);
    }

    stbi_flip_vertically_on_write(true);
    return stbi_write_png(filepath.c_str(), texture.width(), texture.height(), 4, color.data(), texture.width() * 4);
}

template<>
bool load_texture(Texture<RGBA8>& texture, const std::string& filepath)
{
    static_assert(sizeof(RGBA8) == 4*sizeof(unsigned char), "RGBA8 is not tightly packed -- can't initialize from vector of bytes");

    int width = 0, height = 0, components = 0;

    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &components, 4);
    if(data == nullptr) { return false; }

    texture = Texture<RGBA8>(width, height);
    texture.m_mipmaps.reserve(1 + floor(std::log2(std::max(width, height))));

    auto& base_level = texture.m_mipmaps.front();
    base_level.data().resize(width*height);
    std::memcpy(base_level.ptr(), data, width*height*sizeof(RGBA8));

    stbi_image_free(data);

    return true;
}

bool save_texture(const TextureStorage<RGBA8>& texture, const std::string& filepath)
{
    static_assert(sizeof(RGBA8) == 4*sizeof(unsigned char), "RGBA8 is not tightly packed -- needs special handling to allow stbi to write image file!");

    stbi_flip_vertically_on_write(true);
    return stbi_write_png(filepath.c_str(), texture.width(), texture.height(), 4, texture.ptr(), texture.width() * 4);
}

bool load_texture(TextureStorage<RGBA8>& texture, const std::string& filepath)
{
    static_assert(sizeof(RGBA8) == 4*sizeof(unsigned char), "RGBA8 is not tightly packed -- needs special handling to allow stbi to write image file!");

    int width = 0, height = 0, components = 0;

    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &components, 4);
    if(data == nullptr) { return false; }

    texture = TextureStorage<RGBA8>(width, height);
    texture.data().resize(width*height);
    std::memcpy(texture.ptr(), data, width*height*sizeof(RGBA8));

    return true;
}

template<>
bool load_mipmaps(Texture<RGBA8>& texture, const std::string& folder, const std::string& filename)
{
    if(!load_texture(texture, folder + "0_" + filename)) { return false; }

    int max_levels = 1 + floor(std::log2(std::max(texture.width(), texture.height())));
    auto& mipmaps = texture.mipmaps();

    for(int i = 1; i < max_levels; i++)
    {
        mipmaps.emplace_back();
        if(!load_texture(mipmaps.back(), folder + std::to_string(i) + "_" + filename)) { return false; }
    }

    return true;
}

template<>
bool save_mipmaps(const Texture<RGBA8>& texture, const std::string& folder, const std::string& filename)
{
    const auto& mipmaps = texture.mipmaps();

    bool ok = true;
    for(unsigned int level = 0; level < mipmaps.size(); level++)
    {
        ok = ok && save_texture(mipmaps[level], folder + std::to_string(level) + "_" + filename);
    }

    return ok;
}


/*********************************************
 *                                           *
 *             HDR Textures (float)          *
 *                                           *
 *********************************************/
template<>
bool load_texture(Texture<RGBAF>& texture, const std::string& filepath)
{
    static_assert(sizeof(RGBAF) == 4*sizeof(float), "RGBAF is not tightly packed -- can't initialize from vector of floats");

    int width = 0, height = 0, components = 0;

    stbi_set_flip_vertically_on_load(true);
    float* data = stbi_loadf(filepath.c_str(), &width, &height, &components, 4);
    if(data == nullptr) { return false; }


    texture = Texture<RGBAF>(width, height);
    texture.m_mipmaps.reserve(1 + floor(std::log2(std::max(width, height))));

    auto& base_level = texture.m_mipmaps.front();
    base_level.data().resize(width*height);
    std::memcpy(reinterpret_cast<void*>(base_level.ptr()), reinterpret_cast<void*>(data), width*height*sizeof(RGBAF));

    stbi_image_free(data);

    return true;
}

template<>
bool save_texture(const Texture<RGBAF>& texture, const std::string& filepath)
{
    static_assert(sizeof(RGBAF) == 4*sizeof(float), "RGBAF is not tightly packed -- needs special handling to allow stbi to write image file!");

    stbi_flip_vertically_on_write(true);
    return stbi_write_hdr(filepath.c_str(), texture.width(), texture.height(), 4, reinterpret_cast<const float*>(texture.ptr()));
}

bool save_texture(const TextureStorage<RGBAF>& texture, const std::string& filepath)
{
    static_assert(sizeof(RGBAF) == 4*sizeof(float), "RGBAF is not tightly packed -- needs special handling to allow stbi to write image file!");

    stbi_flip_vertically_on_write(true);
    return stbi_write_hdr(filepath.c_str(), texture.width(), texture.height(), 4, reinterpret_cast<const float*>(texture.ptr()));
}

bool load_texture(TextureStorage<RGBAF>& texture, const std::string& filepath)
{
    static_assert(sizeof(RGBA8) == 4*sizeof(unsigned char), "RGBA8 is not tightly packed -- needs special handling to allow stbi to write image file!");

    int width = 0, height = 0, components = 0;

    stbi_set_flip_vertically_on_load(true);
    float* data = stbi_loadf(filepath.c_str(), &width, &height, &components, 4);
    if(data == nullptr) { return false; }

    texture = TextureStorage<RGBAF>(width, height);
    texture.data().resize(width*height);
    std::memcpy(reinterpret_cast<void*>(texture.ptr()), reinterpret_cast<void*>(data), width*height*sizeof(RGBAF));

    return true;
}

template<>
bool save_mipmaps(const Texture<RGBAF>& texture, const std::string& folder, const std::string& filename)
{
    const auto& mipmaps = texture.mipmaps();

    bool ok = true;
    for(unsigned int level = 0; level < mipmaps.size(); level++)
    {
        ok = ok && save_texture(mipmaps[level], folder + std::to_string(level) + "_" + filename);
    }

    return ok;
}

template<>
bool load_mipmaps(Texture<RGBAF>& texture, const std::string& folder, const std::string& filename)
{
    if(!load_texture(texture, folder + "0_" + filename)) { return false; }

    int max_levels = 1 + floor(std::log2(std::max(texture.width(), texture.height())));
    auto& mipmaps = texture.mipmaps();

    for(int i = 1; i < max_levels; i++)
    {
        mipmaps.emplace_back();
        if(!load_texture(mipmaps.back(), folder + std::to_string(i) + "_" + filename)) { return false; }
    }

    return true;
}
