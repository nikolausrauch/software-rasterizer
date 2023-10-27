#include "model.h"

#include <tiny_obj_loader/tiny_obj_loader.h>

namespace asset
{

namespace detail
{

template<typename VertexType>
void loadVertex(VertexType& vertex, const tinyobj::attrib_t& attrib, const tinyobj::index_t& idx, const std::filesystem::path& filepath)
{
    static_assert(detail::has_member<VertexType>::position::value, "Vertex Type needs a position!");
    {
        vertex.position =
        {
            attrib.vertices[3*size_t(idx.vertex_index) + 0],
            attrib.vertices[3*size_t(idx.vertex_index) + 1],
            attrib.vertices[3*size_t(idx.vertex_index) + 2]
        };
    }

    if constexpr (detail::has_member<VertexType>::normal::value)
    {
        if(idx.normal_index < 0)
        {
            throw std::runtime_error("Loaded model does not include normals (" + filepath.string() + ")");
        }

        vertex.normal =
        {
            attrib.normals[3*size_t(idx.normal_index) + 0],
            attrib.normals[3*size_t(idx.normal_index) + 1],
            attrib.normals[3*size_t(idx.normal_index) + 2]
        };
    }

    if constexpr (detail::has_member<VertexType>::texcoord::value)
    {
        if(idx.texcoord_index < 0)
        {
            throw std::runtime_error("Loaded model does not include texture coordinates (" + filepath.string() + ")");
        }

        vertex.texcoord =
        {
            attrib.texcoords[2*size_t(idx.texcoord_index) + 0],
            attrib.texcoords[2*size_t(idx.texcoord_index) + 1]
        };
    }

    if constexpr (detail::has_member<VertexType>::color::value)
    {
        vertex.color =
        {
            attrib.colors[3*size_t(idx.vertex_index) + 0],
            attrib.colors[3*size_t(idx.vertex_index) + 1],
            attrib.colors[3*size_t(idx.vertex_index) + 2]
        };
    }
}

template<typename MaterialType>
void loadMaterial(MaterialType& material, const tinyobj::material_t& loaded, const std::filesystem::path& filepath, bool warnings = false)
{
    if constexpr(detail::has_member<MaterialType>::name::value)
    {
        material.name = loaded.name;
    }

    /*----------------- obj standard -------------------*/
    if constexpr(detail::has_member<MaterialType>::ambient::value)
    {
        material.ambient = { loaded.ambient[0], loaded.ambient[1], loaded.ambient[2] };
    }

    if constexpr(detail::has_member<MaterialType>::diffuse::value)
    {
        material.diffuse = { loaded.diffuse[0], loaded.diffuse[1], loaded.diffuse[2] };
    }

    if constexpr(detail::has_member<MaterialType>::specular::value)
    {
        material.specular = { loaded.specular[0], loaded.specular[1], loaded.specular[2] };
    }

    if constexpr(detail::has_member<MaterialType>::transmittance::value)
    {
        material.transmittance = { loaded.transmittance[0], loaded.transmittance[1], loaded.transmittance[2] };
    }

    if constexpr(detail::has_member<MaterialType>::emission::value)
    {
        material.emission = { loaded.emission[0], loaded.emission[1], loaded.emission[2] };
    }

    if constexpr(detail::has_member<MaterialType>::shininess::value)
    {
        material.shininess = loaded.shininess;
    }

    if constexpr(detail::has_member<MaterialType>::ior::value)
    {
        material.ior = loaded.ior;
    }

    if constexpr(detail::has_member<MaterialType>::dissolve::value)
    {
        material.dissolve = loaded.dissolve;
    }

    if constexpr(detail::has_member<MaterialType>::map_ambient::value)
    {
        material.map_ambient = Texture<RGBA8>(1, 1, RGBA8(255, 255, 255, 255));

        if(warnings && loaded.ambient_texname.empty())
        {
            std::cerr << "[loadObj] ambient map missing [" + loaded.name + "] (" + filepath.string() + ")" << std::endl;
            std::cerr << "[loadObj] default ambient map loaded [" + loaded.name + "]" << std::endl;
        }
        else if(!loaded.ambient_texname.empty())
        {
            auto path = filepath.parent_path();
            path /= loaded.ambient_texname;
            material.map_ambient.load(path.string());
        }
    }

    if constexpr(detail::has_member<MaterialType>::map_diffuse::value)
    {
        material.map_diffuse = Texture<RGBA8>(1, 1, RGBA8(255, 255, 255, 255));

        if(warnings && loaded.diffuse_texname.empty())
        {
            std::cerr << "[loadObj] diffuse map missing [" + loaded.name + "] (" + filepath.string() + ")" << std::endl;
            std::cerr << "[loadObj] default diffuse map loaded [" + loaded.name + "]" << std::endl;
        }
        else if(!loaded.diffuse_texname.empty())
        {
            auto path = filepath.parent_path();
            path /= std::filesystem::path(loaded.diffuse_texname);
            material.map_diffuse.load(path.string());
        }
    }

    if constexpr(detail::has_member<MaterialType>::map_specular::value)
    {
        material.map_specular = Texture<RGBA8>(1, 1, RGBA8(255, 255, 255, 255));

        if(warnings && loaded.specular_texname.empty())
        {
            std::cerr << "[loadObj] secular map missing [" + loaded.name + "] (" + filepath.string() + ")" << std::endl;
            std::cerr << "[loadObj] default secular map loaded [" + loaded.name + "]" << std::endl;
        }
        else if(!loaded.specular_texname.empty())
        {
            auto path = filepath.parent_path();
            path /= std::filesystem::path(loaded.specular_texname);
            material.map_specular.load(path.string());
        }
    }

    if constexpr(detail::has_member<MaterialType>::map_specular_highlight::value)
    {
        material.map_specular_highlight = Texture<RGBA8>(1, 1, RGBA8(0, 0, 0, 255));

        if(warnings && loaded.specular_highlight_texname.empty())
        {
            std::cerr << "[loadObj] specular highlight map missing [" + loaded.name + "] (" + filepath.string() + ")" << std::endl;
            std::cerr << "[loadObj] default highlight map loaded [" + loaded.name + "]" << std::endl;
        }
        else if(!loaded.specular_highlight_texname.empty())
        {
            auto path = filepath.parent_path();
            path /= std::filesystem::path(loaded.specular_highlight_texname);
            material.map_specular_highlight.load(path.string());
        }
    }

    if constexpr(detail::has_member<MaterialType>::map_bump::value)
    {
        material.map_bump = Texture<RGBA8>(1, 1, RGBA8(0, 0, 0, 255));

        if(warnings && loaded.bump_texname.empty())
        {
            std::cerr << "[loadObj] bump map missing [" + loaded.name + "] (" + filepath.string() + ")" << std::endl;
            std::cerr << "[loadObj] default bump map loaded [" + loaded.name + "]" << std::endl;
        }
        else if(!loaded.bump_texname.empty())
        {
            auto path = filepath.parent_path();
            path /= std::filesystem::path(loaded.bump_texname);
            material.map_bump.load(path.string());
        }
    }

    if constexpr(detail::has_member<MaterialType>::map_displacement::value)
    {
        material.map_displacement = Texture<RGBA8>(1, 1, RGBA8(0, 0, 0, 255));

        if(warnings && loaded.displacement_texname.empty())
        {
            std::cerr << "[loadObj] displacement map missing [" + loaded.name + "] (" + filepath.string() + ")" << std::endl;
            std::cerr << "[loadObj] default bump map loaded [" + loaded.name + "]" << std::endl;
        }
        else if(!loaded.displacement_texname.empty())
        {
            auto path = filepath.parent_path();
            path /= std::filesystem::path(loaded.displacement_texname);
            material.map_displacement.load(path.string());
        }
    }


    /*----------------- pbr extension -------------------*/
    if constexpr(detail::has_member<MaterialType>::roughness::value)
    {
        material.roughness = loaded.roughness;
    }

    if constexpr(detail::has_member<MaterialType>::metallic::value)
    {
        material.metallic = loaded.metallic;
    }

    if constexpr(detail::has_member<MaterialType>::sheen::value)
    {
        material.sheen = loaded.sheen;
    }

    if constexpr(detail::has_member<MaterialType>::clearcoat_thickness::value)
    {
        material.clearcoat_thickness = loaded.clearcoat_thickness;
    }

    if constexpr(detail::has_member<MaterialType>::clearcoat_roughness::value)
    {
        material.clearcoat_roughness = loaded.clearcoat_roughness;
    }

    if constexpr(detail::has_member<MaterialType>::anisotropy::value)
    {
        material.anisotropy = loaded.anisotropy;
    }

    if constexpr(detail::has_member<MaterialType>::anisotropy_rotation::value)
    {
        material.anisotropy_rotation = loaded.anisotropy_rotation;
    }

    if constexpr(detail::has_member<MaterialType>::map_albedo::value)
    {
        material.map_albedo = Texture<RGBA8>(1, 1, RGBA8(255, 255, 255, 255));

        if(warnings && loaded.diffuse_texname.empty())
        {
            std::cerr << "[loadObj] albedo map missing [" + loaded.name + "] (" + filepath.string() + ")" << std::endl;
            std::cerr << "[loadObj] default albedo map loaded [" + loaded.name + "]" << std::endl;
        }
        else if(!loaded.diffuse_texname.empty())
        {
            auto path = filepath.parent_path();
            path /= std::filesystem::path(loaded.diffuse_texname);
            material.map_albedo.load(path.string());
        }
    }

    if constexpr(detail::has_member<MaterialType>::map_metallic::value)
    {
        material.map_metallic = Texture<RGBA8>(1, 1, RGBA8(0, 0, 0, 255));

        if(warnings && loaded.metallic_texname.empty())
        {
            std::cerr << "[loadObj] metallic map missing [" + loaded.name + "] (" + filepath.string() + ")" << std::endl;
            std::cerr << "[loadObj] default metallic map loaded [" + loaded.name + "]" << std::endl;
        }
        else if(!loaded.metallic_texname.empty())
        {
            auto path = filepath.parent_path();
            path /= std::filesystem::path(loaded.metallic_texname);
            material.map_metallic.load(path.string());
        }
    }

    if constexpr(detail::has_member<MaterialType>::map_roughness::value)
    {
        material.map_roughness = Texture<RGBA8>(1, 1, RGBA8(255, 255, 255, 255));

        if(warnings && loaded.roughness_texname.empty())
        {
            std::cerr << "[loadObj] roughness map missing [" + loaded.name + "] (" + filepath.string() + ")" << std::endl;
            std::cerr << "[loadObj] default roughness map loaded [" + loaded.name + "]" << std::endl;
        }
        else if(!loaded.roughness_texname.empty())
        {
            auto path = filepath.parent_path();
            path /= std::filesystem::path(loaded.roughness_texname);
            material.map_roughness.load(path.string());
        }
    }

    if constexpr(detail::has_member<MaterialType>::map_normal::value)
    {
        material.map_normal = Texture<RGBA8>(1, 1, RGBA8(0, 0, 0, 255));

        if(warnings && loaded.normal_texname.empty())
        {
            std::cerr << "[loadObj] normal map missing [" + loaded.name + "] (" + filepath.string() + ")" << std::endl;
            std::cerr << "[loadObj] default normal map loaded [" + loaded.name + "]" << std::endl;
        }
        else if(!loaded.normal_texname.empty())
        {
            auto path = filepath.parent_path();
            path /= std::filesystem::path(loaded.normal_texname);
            material.map_normal.load(path.string());
        }
    }


    if constexpr(detail::has_member<MaterialType>::map_metallic_roughness::value)
    {
        material.map_metallic_roughness = Texture<RGBA8>(1, 1, RGBA8(0, 50, 0, 255));

        Texture<RGBA8> map_roughness(1, 1, RGBA8(255, 255, 255, 255));
        Texture<RGBA8> map_metallic(1, 1, RGBA8(0, 0, 0, 255));

        if(warnings && loaded.metallic_texname.empty())
        {
            std::cerr << "[loadObj] metallic map missing [" + loaded.name + "] (" + filepath.string() + ")" << std::endl;

            if(loaded.roughness_texname.empty())
            {
                std::cerr << "[loadObj] roughness map missing [" + loaded.name + "] (" + filepath.string() + ")" << std::endl;
                std::cerr << "[loadObj] default roughness metallic map loaded [" + loaded.name + "]" << std::endl;
            }
            else
            {
                std::cerr << "[loadObj] assumes roughness metallic map is stored in roughness map [" + loaded.name + "] (" + filepath.string() + ")" << std::endl;

                auto path = filepath.parent_path();
                path /= std::filesystem::path(loaded.roughness_texname);
                material.map_metallic_roughness.load(path.string());
            }
        }
        else if(!loaded.metallic_texname.empty() && loaded.roughness_texname.empty())
        {
            std::cerr << "[loadObj] metallic map is used for roughness metallic map [" + loaded.name + "] (" + filepath.string() + ")" << std::endl;
            auto path = filepath.parent_path();
            path /= std::filesystem::path(loaded.metallic_texname);
            material.map_metallic_roughness.load(path.string());
        }
        else if(!loaded.metallic_texname.empty() && !loaded.roughness_texname.empty())
        {
            std::cerr << "[loadObj] metallic and roughness maps are merged [" + loaded.name + "] (" + filepath.string() + ")" << std::endl;

            {
                auto path = filepath.parent_path();
                path /= std::filesystem::path(loaded.roughness_texname);
                map_roughness.load(path.string());
            }

            {
                auto path = filepath.parent_path();
                path /= std::filesystem::path(loaded.roughness_texname);
                map_metallic.load(path.string());
            }

            if( (map_metallic.width() != map_roughness.width())
             || (map_metallic.height() != map_roughness.height()))
            {
                std::cerr << "[loadObj] metallic and roughness maps are not of the same dimensions (required if merging is forced)"  << std::endl;
            }
            else
            {
                /* merge to single texture */
                material.map_metallic_roughness = Texture<RGBA8>(map_metallic.width(), map_metallic.height(), RGBA8(0, 0, 0, 255));
                for(int j = 0; j < map_metallic.height(); j++)
                {
                    for(int i = 0; i < map_metallic.width(); i++)
                    {
                        material.map_metallic_roughness(i, j) = { 0, map_roughness(i,j).x, map_metallic(i,j).x, 255 };
                    }
                }
            }
        }
    }
}

}


/* TODO: this function is quite a mess */
template<typename Mesh>
Model<Mesh> loadObj(const std::filesystem::path& filepath, bool warnings = false)
{
    if(!std::filesystem::exists(filepath))
    {
        throw std::runtime_error("[Model] Cannot find file " + filepath.string());
    }

    tinyobj::ObjReaderConfig config;
    config.triangulate = true;

    tinyobj::ObjReader reader;

    if(!reader.ParseFromFile(filepath.string(), config))
    {
        if(!reader.Error().empty()) std::cerr << "[loadObj] " << reader.Error() << std::endl;
        std::cerr.flush();
        throw std::runtime_error("Error while loading Obj File " + filepath.string());
    }

    if(!reader.Warning().empty())
    {
        std::cerr << "[loadObj] " << reader.Warning() << std::endl;
    }


    Model<Mesh> model;
    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();
    auto& materials = reader.GetMaterials();

    if constexpr (!std::is_empty_v<typename Mesh::MaterialType>)
    {
        if(materials.empty())
        {
            throw std::runtime_error("Loaded Model not include material properties (" + filepath.string() + ")");
        }

        model.materials().reserve(materials.size());
        for(unsigned int i = 0; i < materials.size(); i++)
        {
            detail::loadMaterial(model.materials().emplace_back(), materials[i], filepath, warnings);
        }
    }


    model.meshes().reserve(shapes.size());
    for(auto& shape : shapes)
    {

        if(shape.mesh.num_face_vertices.size() > 0)
        {
            auto& mesh = model.meshes().emplace_back();
            size_t num_faces = shape.mesh.num_face_vertices.size();
            std::unordered_map<unsigned int, unsigned int> materialGroups;
            std::vector<int> origIdx(num_faces * 3, 0);
            std::vector<Vec3> tangents(num_faces * 3, {0, 0, 0});
            std::vector<Vec3> bitangents(num_faces * 3, {0, 0, 0});

            auto& vertices = mesh.vertices();
            vertices.resize(num_faces * 3);

            size_t index_offset = 0;
            for(size_t f = 0; f < num_faces; f++)
            {
                for(size_t v = 0; v < 3; v++)
                {
                    tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
                    origIdx[index_offset + v] = idx.vertex_index;

                    auto& vertex = vertices[index_offset + v];

                    detail::loadVertex(vertex, attrib, idx, filepath);
                }

                if constexpr (!std::is_empty_v<typename Mesh::MaterialType>)
                {
                    auto& meshGroups = mesh.materialGroups();
                    unsigned int matIdx = shape.mesh.material_ids[f];

                    if(!materialGroups.contains(matIdx))
                    {
                        materialGroups[matIdx] = meshGroups.size();
                        meshGroups.emplace_back(model.materials()[matIdx]);
                    }

                    auto& materialIndices = meshGroups[materialGroups[matIdx]];

                    materialIndices.indices.insert(materialIndices.indices.end(), {
                                                       static_cast<unsigned int>(index_offset + 0),
                                                       static_cast<unsigned int>(index_offset + 1),
                                                       static_cast<unsigned int>(index_offset + 2)
                                                   });
                }

                if constexpr (detail::has_member<typename Mesh::VertexType>::tangent::value)
                {
                    if constexpr (! (detail::has_member<typename Mesh::VertexType>::texcoord::value && detail::has_member<typename Mesh::VertexType>::normal::value))
                    {
                        throw std::runtime_error("Tangent space calculation requires texture coordinates and normals (" + filepath.string() + ")");
                    }

                    auto& v1 = vertices[index_offset + 0];
                    auto& v2 = vertices[index_offset + 1];
                    auto& v3 = vertices[index_offset + 2];

                    auto e1 = v2.position - v1.position;
                    auto e2 = v3.position - v1.position;
                    auto uv1 = v2.texcoord - v1.texcoord;
                    auto uv2 = v3.texcoord - v1.texcoord;

                    /* TODO: this needs a cleaner solution (what if uv coordinates are the same) */
                    if(length(uv1) < 1e-8f) { uv1.x = 1e-6f; }
                    if(length(uv2) < 1e-8f) { uv2.y = 1e-6f; }

                    float invDet = 1.0f / std::max(uv1.x * uv2.y - uv2.x * uv1.y, 1e-6f);

                    int idx_1 = shape.mesh.indices[index_offset + 0].vertex_index;
                    int idx_2 = shape.mesh.indices[index_offset + 1].vertex_index;
                    int idx_3 = shape.mesh.indices[index_offset + 2].vertex_index;
                    if constexpr (detail::has_member<typename Mesh::VertexType>::tangent::value)
                    {
                        auto tangent = invDet * (uv2.y * e1 - uv1.y * e2);
                        tangents[idx_1] += tangent;
                        tangents[idx_2] += tangent;
                        tangents[idx_3] += tangent;

                        auto bitangent = invDet * (uv1.x * e2 - uv2.x * e1);
                        bitangents[idx_1] += bitangent;
                        bitangents[idx_2] += bitangent;
                        bitangents[idx_3] += bitangent;
                    }
                }

                index_offset += 3;
            }

            if constexpr (detail::has_member<typename Mesh::VertexType>::tangent::value)
            {
                for(unsigned int i = 0; i < vertices.size(); i++)
                {
                    auto& vertex = vertices[i];
                    int idx = origIdx[i];
                    const auto& tangent = tangents[idx];
                    const auto& bitangent = tangents[idx];
                    const auto& normal = vertex.normal;
                    vertex.tangent = Vec4(normalize(tangent - dot(tangent, normal) * normal), 0.0f);
                    vertex.tangent.w = dot(cross(tangent, bitangent), normal) > 0.0f ? 1.0f : -1.0f;
                }
            }

        }
        else if(shape.points.indices.size() > 0)
        {
            auto& mesh = model.meshes().emplace_back();
            size_t num_points = shape.points.indices.size();

            auto& vertices = mesh.vertices();
            vertices.resize(num_points);

            for(unsigned int p = 0; p < num_points; p++)
            {
                tinyobj::index_t idx = shape.points.indices[p];

                auto& vertex = vertices[p];

                detail::loadVertex(vertex, attrib, idx, filepath);
            }
        }
    }

    return model;
}

}
