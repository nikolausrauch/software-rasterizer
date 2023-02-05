/**
TODO explanation for usage:

iterate through meshes
    bind mesh's vertex array object
    bind mesh's uniform buffer object
    iterate through mesh's materials
    use shaders, bind textures, set uniforms...
    draw material's index buffer with glDrawElements

**/

#pragma once

#include <math/vector2.h>
#include <math/vector3.h>
#include <math/vector4.h>
#include <texture.h>

#include <string>
#include <vector>
#include <map>
#include <list>
#include <unordered_map>
#include <type_traits>
#include <iostream>
#include <filesystem>
#include <variant>

#include <iostream>

namespace asset
{

/*============= All possible Members for Vertex and Material =============*/
struct _AllMaterial
{
    std::string name;

    /* standard obj */
    Vec3 ambient;
    Vec3 diffuse;
    Vec3 specular;
    Vec3 transmittance;
    Vec3 emission;
    float shininess;
    float ior;
    float dissolve;

    Texture<RGBA8> map_ambient;
    Texture<RGBA8> map_diffuse;
    Texture<RGBA8> map_specular;
    Texture<RGBA8> map_specular_highlight;
    Texture<RGBA8> map_bump;
    Texture<RGBA8> map_displacement;
};


struct _AllVertex
{
    Vec3 position;
    Vec3 normal;
    Vec2 texcoord;
    Vec4 tangent;
    Vec3 color;
};



/*========================== Mesh ==========================*/
template<typename Data, typename Mat = std::monostate>
struct Mesh
{
    using IndexType = unsigned int;
    using VertexType = Data;
    using MaterialType = Mat;
    template<typename T> using HasMaterial = std::enable_if_t<!std::is_empty_v<T>>;

    struct MaterialGroup
    {
        MaterialType& material;
        std::vector<IndexType> indices;
    };

public:
    Mesh() = default;
    ~Mesh() = default;

    std::vector<VertexType>& vertices() { return mVertexData; }
    template<typename T = Mat, typename = HasMaterial<T>> T& material(unsigned idx = 0) { assert(idx < mMaterialGroups.size()); return mMaterialGroups[idx].material; }
    template<typename T = Mat, typename = HasMaterial<T>> std::vector<MaterialGroup>& materialGroups() { return mMaterialGroups; }

private:
    std::vector<VertexType> mVertexData;
    std::vector<MaterialGroup> mMaterialGroups;
};

/*========================== Model ==========================*/
template<typename Mesh>
struct Model
{
    using MaterialType = typename Mesh::MaterialType;
    template<typename T> using HasMaterial = std::enable_if_t<!std::is_empty_v<T>>;

    Model() = default;
    ~Model() = default;

    std::vector<Mesh>& meshes() { return mMeshes; }
    template<typename T = MaterialType, typename = HasMaterial<T>> T& material(unsigned int idx = 0) { assert(idx < mMaterials.size()); return mMaterials[idx]; }
    template<typename T = MaterialType, typename = HasMaterial<T>> std::vector<T>& materials() { return mMaterials; }

private:
    std::vector<Mesh> mMeshes;
    std::vector<MaterialType> mMaterials;
};


namespace detail
{

template<typename T>
struct has_member
{
    typedef char one;
    struct two { char _x[2]; };

#define test_one(name) template<typename C> static one test_##name( decltype (&C::name) );
#define test_two(name) template<typename C> static two test_##name(...);
#define test_value(name) struct name{ enum { value = sizeof( test_##name<T>(0) ) == sizeof(char) }; };
#define property(name) test_one(name) test_two(name) test_value(name)

    /* vertex members */
    property(position)
    property(normal)
    property(texcoord)
    property(color)
    property(tangent)

    /* material members */
    property(name);
    property(ambient);
    property(diffuse);
    property(specular);
    property(transmittance);
    property(emission);
    property(shininess);
    property(ior);
    property(dissolve);
    property(map_ambient);
    property(map_diffuse);
    property(map_specular);
    property(map_specular_highlight);
    property(map_bump);
    property(map_displacement);

#undef test_one
#undef test_two
#undef test_value
#undef property
};

}


}
