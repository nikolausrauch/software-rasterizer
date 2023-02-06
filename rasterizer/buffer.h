#pragma once

#include <vector>

enum class ePrimitive
{
    TRIANGLES,
    LINES
};

template <typename Vert>
struct Buffer
{
    ePrimitive primitive = ePrimitive::TRIANGLES;
    std::vector<Vert> vertices;
};


template<typename Vert, typename Ind>
struct BufferIndexed : public Buffer<Vert>
{
    std::vector<Ind> indices;
};
