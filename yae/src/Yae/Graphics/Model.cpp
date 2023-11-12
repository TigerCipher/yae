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

namespace yae::gfx
{

bool model::init()
{
    D3D11_BUFFER_DESC      vertex_buffer_desc{};
    D3D11_BUFFER_DESC      index_buffer_desc{};
    D3D11_SUBRESOURCE_DATA vertex_data{};
    D3D11_SUBRESOURCE_DATA index_data{};

    m_vertex_count = 3;
    m_index_count  = 3;

    std::vector<vertex> vertices{ m_vertex_count };
    std::vector<u32>    indices(m_index_count);

    // bottom left
    vertices[0].position = { -1.f, -1.f, 0.f };
    vertices[0].color    = { 0.f, 1.f, 0.f, 1.f };

    // top middle
    vertices[1].position = { 0.f, 1.f, 0.f };
    vertices[1].color    = { 1.f, 0.f, 0.f, 1.f };

    // bottom right
    vertices[2].position = { 1.f, -1.f, 0.f };
    vertices[2].color    = { 0.f, 0.f, 1.f, 1.f };

    indices[0] = 0; // Bottom left
    indices[1] = 1; // Top middle
    indices[2] = 2; // Bottom right

    vertex_buffer_desc.Usage               = D3D11_USAGE_DEFAULT;
    vertex_buffer_desc.ByteWidth           = sizeof(vertex) * m_vertex_count;
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

void model::render()
{
    constexpr u32 stride = sizeof(vertex);
    constexpr u32 offset = 0;

    core::get_device_context()->IASetVertexBuffers(0, 1, &m_vertex_buffer, &stride, &offset);
    core::get_device_context()->IASetIndexBuffer(m_index_buffer, DXGI_FORMAT_R32_UINT, 0);
    core::get_device_context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

} // namespace yae::gfx