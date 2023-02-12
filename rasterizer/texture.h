#pragma once

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
struct Texture
{
    Texture(unsigned int width = 1, unsigned int height = 1, const T& value = T())
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

    bool load(const std::string& filepath)
    {
        return load_texture<T>(*this, filepath);
    }

    bool save(const std::string& filepath) const
    {
        return save_texture<T>(*this, filepath);
    }

private:
    int m_width;
    int m_height;
    std::vector<T> m_values;

    friend bool load_texture<>(Texture<T>& texture, const std::string& filepath);
    friend bool save_texture<>(const Texture<T>& texture, const std::string& filepath);
};

template<typename T>
bool load_texture(Texture<T>& texture, const std::string& filepath)
{
    (void) texture; (void) filepath;
    std::cerr << "[Texture] No specialized loading function load_texture<T> for this Texture type" << std::endl;
    return false;
}

template<typename T>
bool save_texture(const Texture<T>& texture, const std::string& filepath)
{
    (void) texture; (void) filepath;
    std::cerr << "[Texture] No specialized saving function save_texture<T> for this Texture type" << std::endl;
    return false;
}


typedef Vector4<std::uint8_t> RGBA8;
template<> bool load_texture(Texture<RGBA8>& texture, const std::string& filepath);
template<> bool save_texture(const Texture<RGBA8>& texture, const std::string& filepath);

typedef float Depth;
template<> bool save_texture(const Texture<Depth>& texture, const std::string& filepath);
