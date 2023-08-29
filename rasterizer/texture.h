#pragma once

#include "math/vector2.h"
#include "math/vector3.h"
#include "math/vector4.h"

#include <vector>
#include <string>
#include <cassert>

template<typename T>
struct Texture;

template<typename T>
bool load_texture(Texture<T>& texture, const std::string& filepath);

template<typename T>
bool save_texture(const Texture<T>& texture, const std::string& filepath);

template<typename T>
bool load_mipmaps(Texture<T>& texture, const std::string& folder, const std::string& filename);

template<typename T>
bool save_mipmaps(const Texture<T>& texture, const std::string& folder, const std::string& filename);

template<typename T>
struct TextureStorage
{
    TextureStorage(unsigned int width = 1, unsigned int height = 1, const T& value = T())
        : m_width(width), m_height(height)
    {
        m_values.resize(width * height);
        fill(value);
    }

    void fill(const T& v)
    {
        std::fill(m_values.begin(), m_values.end(), v);
    }

    T& operator()(int i, int j)
    {
        assert(i >= 0 && i < m_width);
        assert(j >= 0 && j < m_height);

        return m_values[j * m_width + i];
    }

    const T& operator()(int i, int j) const
    {
        assert(i >= 0 && i < m_width);
        assert(j >= 0 && j < m_height);

        return m_values[j * m_width + i];
    }

    void data(const std::vector<T>& values) { assert(values.size() == m_width*m_height); m_values = values; }
    std::vector<T>& data() { return m_values; }
    const std::vector<T>& data() const { return m_values; }

    T* ptr() { return m_values.data(); }
    const T* ptr() const { return m_values.data(); }

    int width() const { return m_width; }
    int height() const { return m_height; }

private:
    int m_width;
    int m_height;
    std::vector<T> m_values;
};


template<typename T>
struct Texture
{
    Texture(unsigned int width = 1, unsigned int height = 1, const T& value = T())
    {
        m_mipmaps.reserve(1 + floor(std::log2(std::max(width, height))));
        m_mipmaps.emplace_back(width, height, value);
    }

    void fill(const T& v)
    {
        m_mipmaps.front().fill(v);
    }

    T& operator()(int i, int j)
    {
        return m_mipmaps[0](i, j);
    }

    const T& operator()(int i, int j) const
    {
        return m_mipmaps[0](i, j);
    }

    void data(const std::vector<T>& values) { assert(values.size() == width()*height()); m_mipmaps[0].data() = values; }
    std::vector<T>& data() { return m_mipmaps[0].data(); }
    const std::vector<T>& data() const {  return m_mipmaps[0].data(); }

    T* ptr() { return m_mipmaps[0].data().data(); }
    const T* ptr() const { return m_mipmaps[0].data().data(); }

    int width() const { return m_mipmaps[0].width(); }
    int height() const { return m_mipmaps[0].height(); }
    int num_mipmaps() const { return m_mipmaps.size(); }

    bool load(const std::string& filepath)
    {
        return load_texture<T>(*this, filepath);
    }

    bool save(const std::string& filepath) const
    {
        return save_texture<T>(*this, filepath);
    }

    void generate_mipmaps()
    {
        int level_width = width();
        int level_height = height();
        int max_levels = 1 + floor(std::log2(std::max(level_width, level_height)));

        for(int i = 0; i < max_levels; i++)
        {
            level_width = std::max(1, level_width / 2);
            level_height = std::max(1, level_height / 2);

            const auto& previous_map = m_mipmaps[i];
            auto& level_map = m_mipmaps.emplace_back(level_width, level_height);

            for(int y = 0; y < level_height; y++)
            {
                for(int x = 0; x < level_width; x++)
                {
                    /* average previous level's values */
                    /* TODO: this feels hacky; need accumulation type for uint8 etc. to avoid overflow */
                    decltype(T() * 1.0f) value = decltype(T() * 1.0f)();
                    for(int yy = 0; yy < 2; yy++)
                    {
                        for(int xx = 0; xx < 2; xx++)
                        {
                            int px = std::min(x*2 + xx, previous_map.width() - 1);
                            int py = std::min(y*2 + yy, previous_map.height() - 1);
                            value += previous_map(px, py);
                        }
                    }
                    value /= 2*2;

                    level_map(x, y) = value;
                }
            }
        }
    }

    const std::vector<TextureStorage<T>>& mipmaps() const
    {
        return m_mipmaps;
    }

    std::vector<TextureStorage<T>>& mipmaps()
    {
        return m_mipmaps;
    }

private:
    std::vector<TextureStorage<T>> m_mipmaps;

    friend bool load_texture<>(Texture<T>& texture, const std::string& filepath);
    friend bool save_texture<>(const Texture<T>& texture, const std::string& filepath);
    friend bool load_mipmaps<>(Texture<T>& texture, const std::string& folder, const std::string& filename);
    friend bool save_mipmaps<>(const Texture<T>& texture, const std::string& folder, const std::string& filename);
};

template<typename T>
bool load_texture(Texture<T>& texture, const std::string& filepath)
{
    (void) texture; (void) filepath;
    std::cerr << "[Texture] No specialized loading function load_texture<T> for this Texture type" << std::endl;
    return false;
}

template<typename T>
bool load_mipmaps(Texture<T>& texture, const std::string& folder, const std::string& filename)
{
    (void) texture; (void) folder; (void) filename;
    std::cerr << "[Texture] No specialized loading function load_mipmaps<T> for this Texture type" << std::endl;
    return false;
}

template<typename T>
bool save_texture(const Texture<T>& texture, const std::string& filepath)
{
    (void) texture; (void) filepath;
    std::cerr << "[Texture] No specialized saving function save_texture<T> for this Texture type" << std::endl;
    return false;
}

template<typename T>
bool save_mipmaps(const Texture<T>& texture, const std::string& folder, const std::string& filename)
{
    (void) texture; (void) folder; (void) filename;
    std::cerr << "[Texture] No specialized saving function save_mipmaps<T> for this Texture type" << std::endl;
    return false;
}



typedef Vector4<std::uint8_t> RGBA8;
template<> bool load_texture(Texture<RGBA8>& texture, const std::string& filepath);
template<> bool save_texture(const Texture<RGBA8>& texture, const std::string& filepath);
template<> bool load_mipmaps(Texture<RGBA8>& texture, const std::string& folder, const std::string& filename);
template<> bool save_mipmaps(const Texture<RGBA8>& texture, const std::string& folder, const std::string& filename);

typedef Vector4<float> RGBAF;
template<> bool load_texture(Texture<RGBAF>& texture, const std::string& filepath);
template<> bool save_texture(const Texture<RGBAF>& texture, const std::string& filepath);
template<> bool load_mipmaps(Texture<RGBAF>& texture, const std::string& folder, const std::string& filename);
template<> bool save_mipmaps(const Texture<RGBAF>& texture, const std::string& folder, const std::string& filename);

typedef float Depth;
template<> bool save_texture(const Texture<Depth>& texture, const std::string& filepath);
