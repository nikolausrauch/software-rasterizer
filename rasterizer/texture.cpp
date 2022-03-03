#include "texture.h"

#include <stb_image/stb_image_write.h>
#include <stb_image/stb_image.h>

#include <cstring>

template<>
bool save_texture(const Texture<Color> &texture, const std::string &filepath)
{
    stbi_flip_vertically_on_write(true);
    return stbi_write_png(filepath.c_str(), texture.width(), texture.height(), 4, texture.ptr(), texture.width() * 4);
}

template<>
bool save_texture(const Texture<Depth> &texture, const std::string &filepath)
{
    std::vector<Color> color(texture.width() * texture.height());
    for(unsigned int i = 0; i < color.size(); i++)
    {
        std::uint8_t value = 255.0f * std::clamp<float>(texture.data()[i], 0.0f, 1.0f);
        color[i] = Color(value, value, value, 255);
    }

    stbi_flip_vertically_on_write(true);
    return stbi_write_png(filepath.c_str(), texture.width(), texture.height(), 4, color.data(), texture.width() * 4);
}

template<>
bool load_texture(Texture<Color> &texture, const std::string &filepath)
{
    int width = 0, height = 0, components = 0;

    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &components, 4);
    if(data == nullptr) { return false; }

    texture.m_width = width;
    texture.m_height = height;
    texture.data().resize(width*height);
    std::memcpy(texture.ptr(), data, width*height*4*sizeof(unsigned char));

    stbi_image_free(data);

    return true;
}
