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
//  File Name: Bitmap.cpp
//  Date File Created: 11/18/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------
#include "Bitmap.h"

#include "Yae/Core/System.h"

namespace yae::gfx
{
bitmap::~bitmap()
{
    shutdown();
}

bool bitmap::init(const char* filename, i32 x, i32 y)
{
    m_x = x;
    m_y = y;

    m_vertex_count = 6; // TODO: try 4
    m_index_count  = 6;

    const std::vector<vertex_position_texture> vertices{ m_vertex_count };
    std::vector<u32>                           indices(m_index_count);
    for (u32 i = 0; i < m_index_count; ++i)
    {
        indices[i] = i;
    }

    D3D11_BUFFER_DESC      vertex_buffer_desc{};
    D3D11_BUFFER_DESC      index_buffer_desc{};
    D3D11_SUBRESOURCE_DATA vertex_data{};
    D3D11_SUBRESOURCE_DATA index_data{};

    vertex_buffer_desc.Usage               = D3D11_USAGE_DYNAMIC;
    vertex_buffer_desc.ByteWidth           = sizeof(vertex_position_texture) * m_vertex_count;
    vertex_buffer_desc.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
    vertex_buffer_desc.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
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

    if (!m_texture.init(filename))
    {
        return false;
    }

    m_width  = m_texture.width();
    m_height = m_texture.height();

    return true;
}

void bitmap::shutdown()
{
    core::release(m_vertex_buffer);
    core::release(m_index_buffer);
}

bool bitmap::update()
{
    if (m_old_x == m_x && m_old_y == m_y)
    {
        return true;
    }

    m_old_x = m_x;
    m_old_y = m_y;

    f32 left   = -(f32) system::width() / 2.f + (f32) m_x;
    f32 right  = left + (f32) m_width;
    f32 top    = (f32) system::height() / 2.f - (f32) m_y;
    f32 bottom = top - (f32) m_height;

    if (m_coords_are_center)
    {
        left   = left - (f32) m_width / 2.f;
        right  = left + (f32) m_width;
        top    = top + (f32) m_height / 2.f;
        bottom = top - (f32) m_height;
    }


    std::vector<vertex_position_texture> vertices{ m_vertex_count };

    vertices[0].position = { left, top, 0.0f }; // Top left.
    vertices[0].texture  = { 0.0f, 0.0f };

    vertices[1].position = { right, bottom, 0.0f }; // Bottom right.
    vertices[1].texture  = { 1.0f, 1.0f };

    vertices[2].position = { left, bottom, 0.0f }; // Bottom left.
    vertices[2].texture  = { 0.0f, 1.0f };

    // Second triangle.
    vertices[3].position = { left, top, 0.0f }; // Top left.
    vertices[3].texture  = { 0.0f, 0.0f };

    vertices[4].position = { right, top, 0.0f }; // Top right.
    vertices[4].texture  = { 1.0f, 0.0f };

    vertices[5].position = { right, bottom, 0.0f }; // Bottom right.
    vertices[5].texture  = { 1.0f, 1.0f };


    D3D11_MAPPED_SUBRESOURCE mapped_res{};

    DX_CALL(core::get_device_context()->Map(m_vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_res));
    auto ptr = (vertex_position_texture*) mapped_res.pData;
    CopyMemory(ptr, vertices.data(), sizeof(vertex_position_texture) * vertices.size());
    core::get_device_context()->Unmap(m_vertex_buffer, 0);


    return true;
}

bool bitmap::render(shader* shader, const math::matrix& world)
{
    if (!update())
    {
        return false;
    }
    shader->set_renderer(render_2d);
    shader->set_texture(m_texture.texture_view());
    constexpr u32 stride = sizeof(vertex_position_texture);
    constexpr u32 offset = 0;
    core::get_device_context()->IASetVertexBuffers(0, 1, &m_vertex_buffer, &stride, &offset);
    core::get_device_context()->IASetIndexBuffer(m_index_buffer, DXGI_FORMAT_R32_UINT, 0);
    core::get_device_context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    if (!shader->render(m_index_count, world))
    {
        return false;
    }

    shader->set_renderer(render_3d);
    return true;
}

void bitmap::set_location(i32 x, i32 y)
{
    m_x = x;
    m_y = y;
}


} // namespace yae::gfx