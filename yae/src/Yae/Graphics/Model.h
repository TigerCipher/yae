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
//  File Name: Model.h
//  Date File Created: 11/12/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#pragma once

#include "D3D11Common.h"
#include "D3D11Core.h"
#include "Vertex.h"


template<typename T>
concept vertex_type = is_subclass<T, yae::gfx::vertex>;

namespace yae::gfx
{
class shader;

class model
{
public:
    model()          = default;
    virtual ~model()
    {
        shutdown();
    }

    template<vertex_type VertexType>
    bool init(const std::vector<VertexType>& vertices, const std::vector<u32> indices)
    {
        m_stride       = sizeof(VertexType);
        m_vertex_count = (u32) vertices.size();
        m_index_count  = (u32) indices.size();

        D3D11_BUFFER_DESC      vertex_buffer_desc{};
        D3D11_BUFFER_DESC      index_buffer_desc{};
        D3D11_SUBRESOURCE_DATA vertex_data{};
        D3D11_SUBRESOURCE_DATA index_data{};


        vertex_buffer_desc.Usage               = D3D11_USAGE_DEFAULT;
        vertex_buffer_desc.ByteWidth           = sizeof(VertexType) * m_vertex_count;
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

    bool init(const std::string_view filename);

    void shutdown();

    void bind() const;

    constexpr u32 index_count() const { return m_index_count; }

private:
    ID3D11Buffer* m_vertex_buffer{};
    ID3D11Buffer* m_index_buffer{};
    u32           m_vertex_count{};
    u32           m_index_count{};
    u32           m_stride{};
};


} // namespace yae::gfx