//  ------------------------------------------------------------------------------
//
//  yae
//     Copyright 2023 Matthew Rogers
//
//     Licensed under the Apache License, Version 2.0 (the "License");
//     you may not use this file except in compliance with the License.
//     You may obtain a copy of the License at
//
//         http://www.apache.org/licenses/LICENSE-2.0
//
//     Unless required by applicable law or agreed to in writing, software
//     distributed under the License is distributed on an "AS IS" BASIS,
//     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//     See the License for the specific language governing permissions and
//     limitations under the License.
//
//  File Name: Model.cpp
//  Date File Created: 11/12/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------
#include "Model.h"

#include "D3D11Core.h"
#include "Shaders/Shader.h"

#include <fstream>

namespace yae::gfx
{

namespace
{

std::vector<vertex_pos_norm_tex_tang> load_model(const char* filename)
{
    LOG_INFO("Loading model from {}", filename);
    std::ifstream fin{ filename };
    if (fin.fail())
    {
        LOG_ERROR("Failed to open model file");
        return {};
    }

    char input;
    fin.get(input);
    while (input != ':')
    {
        fin.get(input);
    }

    u32 vertex_count;
    fin >> vertex_count;

    std::vector<vertex_pos_norm_tex_tang> model{ vertex_count };

    fin.get(input);
    while (input != ':')
    {
        fin.get(input);
    }

    fin.get(input);
    fin.get(input);

    for (u32 i = 0; i < vertex_count; ++i)
    {
        fin >> model[i].position.x >> model[i].position.y >> model[i].position.z;
        fin >> model[i].texture.x >> model[i].texture.y;
        fin >> model[i].normal.x >> model[i].normal.y >> model[i].normal.z;
    }

    fin.close();

    LOG_INFO("Done.");
    return model;
}

u32 total_models{};

} // anonymous namespace

model::model()
{
    total_models++;
}

bool model::init(const std::string_view filename)
{
    m_stride = sizeof(vertex_pos_norm_tex_tang);

    std::vector<vertex_pos_norm_tex_tang> vertices = load_model(filename.data());
    if (vertices.empty())
    {
        return false;
    }

    m_vertex_count = (u32) vertices.size();
    m_index_count  = m_vertex_count;

    calculate_vectors(vertices);

    std::vector<u32> indices(m_index_count);
    for (u32 i = 0; i < m_index_count; ++i)
    {
        indices[i] = i;
    }

    D3D11_BUFFER_DESC      vertex_buffer_desc{};
    D3D11_BUFFER_DESC      index_buffer_desc{};
    D3D11_SUBRESOURCE_DATA vertex_data{};
    D3D11_SUBRESOURCE_DATA index_data{};


    vertex_buffer_desc.Usage               = D3D11_USAGE_DEFAULT;
    vertex_buffer_desc.ByteWidth           = m_stride * m_vertex_count;
    vertex_buffer_desc.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
    vertex_buffer_desc.CPUAccessFlags      = 0;
    vertex_buffer_desc.MiscFlags           = 0;
    vertex_buffer_desc.StructureByteStride = 0;

    vertex_data.pSysMem          = vertices.data();
    vertex_data.SysMemPitch      = 0;
    vertex_data.SysMemSlicePitch = 0;

    DX_CALL(core::get_device()->CreateBuffer(&vertex_buffer_desc, &vertex_data, &m_vertex_buffer));

    index_buffer_desc.Usage               = D3D11_USAGE_DEFAULT;
    index_buffer_desc.ByteWidth           = sizeof(u32) * m_index_count;
    index_buffer_desc.BindFlags           = D3D11_BIND_INDEX_BUFFER;
    index_buffer_desc.CPUAccessFlags      = 0;
    index_buffer_desc.MiscFlags           = 0;
    index_buffer_desc.StructureByteStride = 0;

    index_data.pSysMem          = indices.data();
    index_data.SysMemPitch      = 0;
    index_data.SysMemSlicePitch = 0;

    DX_CALL(core::get_device()->CreateBuffer(&index_buffer_desc, &index_data, &m_index_buffer));

    return true;
}

void model::shutdown()
{
    core::release(m_index_buffer);
    core::release(m_vertex_buffer);
    LOG_DEBUG("Shutting down model, {} models remaining", --total_models);
}

void model::bind() const
{
    constexpr u32 offset = 0;
    core::get_device_context()->IASetVertexBuffers(0, 1, &m_vertex_buffer, &m_stride, &offset);
    core::get_device_context()->IASetIndexBuffer(m_index_buffer, DXGI_FORMAT_R32_UINT, 0);
    core::get_device_context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}


void model::calculate_vectors(std::vector<vertex_pos_norm_tex_tang>& vertices)
{
    const u32                      face_count = m_vertex_count / 3;
    u32                            index{};
    vertex_position_normal_texture vtx1, vtx2, vtx3;
    math::vec3                     tangent{};
    math::vec3                     binormal{};

    for (u32 i = 0; i < face_count; ++i)
    {
        vtx1.position = vertices[index].position;
        vtx1.texture  = vertices[index].texture;
        ++index;

        vtx2.position = vertices[index].position;
        vtx2.texture  = vertices[index].texture;
        ++index;

        vtx3.position = vertices[index].position;
        vtx3.texture  = vertices[index].texture;
        ++index;

        calculate_tangents(vtx1, vtx2, vtx3, tangent, binormal);

        vertices[index - 1].tangent  = tangent;
        vertices[index - 1].binormal = binormal;

        vertices[index - 2].tangent  = tangent;
        vertices[index - 2].binormal = binormal;

        vertices[index - 3].tangent  = tangent;
        vertices[index - 3].binormal = binormal;
    }
}


void model::calculate_tangents(const vertex_position_normal_texture& vtx1, const vertex_position_normal_texture& vtx2,
                               const vertex_position_normal_texture& vtx3, math::vec3& tangent, math::vec3& binormal)
{
    math::vec3 vec1 = { vtx2.position.x - vtx1.position.x, vtx2.position.y - vtx1.position.y, vtx2.position.z - vtx1.position.z };
    math::vec3 vec2 = { vtx3.position.x - vtx1.position.x, vtx3.position.y - vtx1.position.y, vtx3.position.z - vtx1.position.z };

    math::vec2 u = { vtx2.texture.x - vtx1.texture.x, vtx2.texture.y - vtx1.texture.y };
    math::vec2 v = { vtx3.texture.x - vtx1.texture.x, vtx3.texture.y - vtx1.texture.y };

    f32 den = 1.f / (u.x * v.y - u.y * v.x);

    tangent.x = (v.y * vec1.x - v.x * vec2.x) * den;
    tangent.y = (v.y * vec1.y - v.x * vec2.y) * den;
    tangent.z = (v.y * vec1.z - v.x * vec2.z) * den;

    binormal.x = (u.x * vec2.x - u.y * vec1.x) * den;
    binormal.y = (u.x * vec2.y - u.y * vec1.y) * den;
    binormal.z = (u.x * vec2.z - u.y * vec1.z) * den;

    f32 len = sqrt(tangent.x * tangent.x + tangent.y * tangent.y + tangent.z * tangent.z);
    tangent.x /= len;
    tangent.y /= len;
    tangent.z /= len;

    len = sqrt(binormal.x * binormal.x + binormal.y * binormal.y + binormal.z * binormal.z);
    binormal.x /= len;
    binormal.y /= len;
    binormal.z /= len;
}

} // namespace yae::gfx