/**
 * TODO:
 * - shadow mapping
 * - discarding of primitives and fragments (clipping, culling)
 * - wireframe rendering of triangles
 * - linear interpolation for lines
 * - cleanup math functions (fix ambiguity)
 */

#pragma once

#include "buffer.h"
#include "program.h"
#include "framebuffer.h"

#include "math/vector2.h"
#include "math/vector4.h"
#include "math/rectangle.h"

#include "detail/test_member.h"
#include "detail/tuple_iter.h"

struct Renderer
{
    Renderer(unsigned int width, unsigned int height);

    Framebuffer& framebuffer();

    template<typename Vertex, typename Varying, typename Uniforms>
    void draw(const Program<Vertex, Varying, Uniforms>& program, const struct Buffer<Vertex>& buffer)
    {
        draw(program, buffer, m_framebuffer);
    }

    template<typename Vertex, typename Varying, typename Uniforms, typename Indx>
    void draw(const Program<Vertex, Varying, Uniforms>& program, const struct BufferIndexed<Vertex, Indx>& buffer)
    {
        draw(program, buffer, m_framebuffer);
    }

    template<typename Vertex, typename Varying, typename Uniforms>
    void draw(const Program<Vertex, Varying, Uniforms>& program, const struct Buffer<Vertex>& buffer, Framebuffer& fb)
    {
        assert(program.m_vertShader);
        assert(program.m_fragShader);

        std::vector<Varying> pipeline_data(buffer.vertices.size());
        process_vertices(buffer.vertices, pipeline_data, program);

        switch(buffer.primitive)
        {
        case ePrimitive::TRIANGLES: draw_triangles(pipeline_data, program, fb);
        }
    }

    template<typename Vertex, typename Varying, typename Uniforms, typename Indx>
    void draw(const Program<Vertex, Varying, Uniforms>& program, const struct BufferIndexed<Vertex, Indx>& buffer, Framebuffer& fb)
    {
        assert(program.m_vertShader);
        assert(program.m_fragShader);

        std::vector<Varying> pipeline_data(buffer.indices.size());
        process_vertices(buffer.vertices, buffer.indices, pipeline_data, program);

        switch(buffer.primitive)
        {
        case ePrimitive::TRIANGLES: draw_triangles(pipeline_data, program, fb);
        }
    }

private:
    template<typename Vertex, typename Varying, typename Uniforms>
    void process_vertices(const std::vector<Vertex>& vertices, std::vector<Varying>& out, const Program<Vertex, Varying, Uniforms>& program)
    {
        for(unsigned int i = 0; i < vertices.size(); i++)
        {
            program.m_vertShader(program.m_uniforms, vertices[i], out[i]);

            // perspective divide
            out[i].position.w = 1.0f / out[i].position.w;
            out[i].position.x *= out[i].position.w;
            out[i].position.y *= out[i].position.w;
            out[i].position.z *= out[i].position.w;

            // screen space (viewport mapping)
            out[i].position.x = m_options.viewport.min.x + (out[i].position.x + 1.0f) / 2.0f * (m_options.viewport.max.x - m_options.viewport.min.x);
            out[i].position.y = m_options.viewport.min.y + (out[i].position.y + 1.0f) / 2.0f * (m_options.viewport.max.y - m_options.viewport.min.y);
        }
    }

    template<typename Vertex, typename Varying, typename Uniforms, typename Indx>
    void process_vertices(const std::vector<Vertex>& vertices, const std::vector<Indx>& indices, std::vector<Varying>& out, const Program<Vertex, Varying, Uniforms>& program)
    {
        for(unsigned int i = 0; i < indices.size(); i++)
        {
            program.m_vertShader(program.m_uniforms, vertices[ indices[i] ], out[i]);

            // perspective divide
            out[i].position.w = 1.0f / out[i].position.w;
            out[i].position.x *= out[i].position.w;
            out[i].position.y *= out[i].position.w;
            out[i].position.z *= out[i].position.w;

            // screen space (viewport mapping)
            out[i].position.x = m_options.viewport.min.x + (out[i].position.x + 1.0f) / 2.0f * (m_options.viewport.max.x - m_options.viewport.min.x);
            out[i].position.y = m_options.viewport.min.y + (out[i].position.y + 1.0f) / 2.0f * (m_options.viewport.max.y - m_options.viewport.min.y);
        }
    }

    template<typename Vertex, typename Varying, typename Uniforms>
    void draw_triangles(const std::vector<Varying>& in, const Program<Vertex, Varying, Uniforms>& program, Framebuffer& fb)
    {
        for(unsigned int i = 0; i < in.size() / 3; i++)
        {
            draw_triangle(in[i*3 + 0], in[i*3 + 1], in[i*3 + 2], program, fb);
        }
    }

    template<typename Vertex, typename Varying, typename Uniforms>
    void draw_triangle(const Varying& v_0, const Varying& v_1, const Varying& v_2, const Program<Vertex, Varying, Uniforms>& program, Framebuffer& fb)
    {
        if(m_options.culling)
        {
            auto normal = cross(Vec3(v_1.position) - Vec3(v_0.position), Vec3(v_2.position) - Vec3(v_0.position));
            if(normal.z < 0.0f) return;
        }

        Recti bbox(v_0.position, v_1.position, v_2.position);
        bbox.clamp(m_options.viewport, 0, -1);

        for(int x = bbox.min.x; x <= bbox.max.x; x++)
        {
            for(int y = bbox.min.y; y <= bbox.max.y; y++)
            {
                Vec2 fragCoord = Vec2(x + 0.5f, y + 0.5f);
                auto bc = barycentric(Vec2(v_0.position), Vec2(v_1.position), Vec2(v_2.position), fragCoord);
                if(bc.x < 0 || bc.y < 0 || bc.z < 0 || std::isnan(bc.x)) continue;

                // linear interpolate in screen space
                float w = bc.x * v_0.position.w + bc.y * v_1.position.w + bc.z * v_2.position.w;
                float z = bc.x * v_0.position.z + bc.y * v_1.position.z + bc.z * v_2.position.z;
                z = z * 0.5f + 0.5f;

                // TODO: culling, clipping should happen earlier
                if(0.0f > z || z > 1.0f) continue;

                // early depth test
                auto& depth = m_framebuffer.depth()(x, y);
                if(z > depth) continue;
                depth = z;

                // perspective correction of barycentric coordinates
                bc.x = 1.0f / w * bc.x * v_0.position.w;
                bc.y = 1.0f / w * bc.y * v_1.position.w;
                bc.z = 1.0f / w * bc.z * v_2.position.w;

                // interpolate fragment data
                Varying inter;
                interpolate_frag_data(bc, v_0, v_1, v_2, inter);

                // call fragment shader
                Vec4 fragColor(0, 0, 0, 0);
                program.m_fragShader(program.m_uniforms, inter, fragColor);

                fb.color()(x, y) = Color( max( min(fragColor, 1.0), 0.0) * 255);
            }
        }
    }

    template<typename Varying>
    void interpolate_frag_data(const Vec3& bc, const Varying& v_0, const Varying& v_1, const Varying& v_2, Varying& result)
    {
        #define VARYING(...) decltype(std::tie( __VA_ARGS__ )) _reflect = std::tie( __VA_ARGS__ );

        auto interpolate = [bc](const auto& x0, const auto& x1, const auto& x2, auto& res)
        {
            res = bc.x * x0 + bc.y * x1 + bc.z * x2;
        };

        detail::iter_tuple(interpolate, v_0._reflect, v_1._reflect, v_2._reflect, result._reflect);
    }

private:
    Framebuffer m_framebuffer;

    struct Options
    {
        Rectf viewport;
        bool culling;
    } m_options;
};
