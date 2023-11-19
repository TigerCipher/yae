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

std::vector<vertex_position_texture_normal> load_model(const char* filename)
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

    std::vector<vertex_position_texture_normal> model{ vertex_count };

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

} // anonymous namespace

bool model::init(const std::string_view filename)
{
    m_stride = sizeof(vertex_position_texture_normal);

    const std::vector<vertex_position_texture_normal> vertices = load_model(filename.data());
    if (vertices.empty())
    {
        return false;
    }


    m_vertex_count = (u32) vertices.size();
    m_index_count  = m_vertex_count;

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
    vertex_buffer_desc.ByteWidth           = sizeof(vertex_position_texture_normal) * m_vertex_count;
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
}


bool model::render(shader* shader, const math::matrix& world) const
{
    constexpr u32 offset = 0;

    core::get_device_context()->IASetVertexBuffers(0, 1, &m_vertex_buffer, &m_stride, &offset);
    core::get_device_context()->IASetIndexBuffer(m_index_buffer, DXGI_FORMAT_R32_UINT, 0);
    core::get_device_context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    if(!shader->render(m_index_count, world))
    {
        return false;
    }

    return true;
}

} // namespace yae::gfx