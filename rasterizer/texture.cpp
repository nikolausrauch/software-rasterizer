#include "texture.h"

#include <stb_image/stb_image_write.h>
#include <stb_image/stb_image.h>

#include <cstring>
#include <algorithm>

template<>
bool save_texture(const Texture<RGBA8> &texture, const std::string &filepath)
{
    stbi_flip_vertically_on_write(true);
    return stbi_write_png(filepath.c_str(), texture.width(), texture.height(), 4, texture.ptr(), texture.width() * 4);
}

template<>
bool save_texture(const Texture<Depth> &texture, const std::string &filepath)
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
bool load_texture(Texture<RGBA8> &texture, const std::string &filepath)
{
    int width = 0, height = 0, components = 0;

    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &components, 4);
    if(data == nullptr) { return false; }

    texture.m_mipmaps.reserve(1 + floor(std::log2(std::max(width, height))));
    auto& base_level = texture.m_mipmaps.front();

    base_level.m_width = width;
    base_level.m_height = height;
    base_level.data().resize(width*height);
    std::memcpy(base_level.ptr(), data, width*height*4*sizeof(unsigned char));

    stbi_image_free(data);

    return true;
}

bool save_texture(const TextureStorage<RGBA8> &texture, const std::string &filepath)
{
    stbi_flip_vertically_on_write(true);
    return stbi_write_png(filepath.c_str(), texture.width(), texture.height(), 4, texture.ptr(), texture.width() * 4);
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
