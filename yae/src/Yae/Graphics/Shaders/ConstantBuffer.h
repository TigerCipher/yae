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
//  File Name: ConstantBuffer.h
//  Date File Created: 11/15/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#pragma once

#include "../D3D11Common.h"
#include "../D3D11Core.h"

namespace yae::gfx
{

namespace cb
{
struct matrix_buffer
{
    math::matrix world;
    math::matrix view;
    math::matrix projection;
};

struct light_buffer
{
    math::vec4 ambient_color;
    math::vec4 diffuse_color;
    math::vec3 direction;
    f32        specular_power;
    math::vec4 specular_color;
};

struct camera_buffer
{
    math::vec3 position;
    f32        padding{};
};
} // namespace cb

enum shader_type
{
    shader_vertex,
    shader_pixel,
};

template<typename T, shader_type Shader>
class constant_buffer
{
public:
    constant_buffer(u32 pos) : m_position{ pos } {}
    ~constant_buffer() { release(); }

    bool init()
    {
        D3D11_BUFFER_DESC desc;
        desc.Usage               = D3D11_USAGE_DYNAMIC;
        desc.ByteWidth           = sizeof(T);
        desc.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
        desc.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
        desc.MiscFlags           = 0;
        desc.StructureByteStride = 0;

        DX_CALL(core::get_device()->CreateBuffer(&desc, nullptr, &m_buffer));

        return true;
    }

    void release() { core::release(m_buffer); }

    bool apply()
    {
        D3D11_MAPPED_SUBRESOURCE mapped_res{};
        DX_CALL(core::get_device_context()->Map(m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_res));
        CopyMemory(mapped_res.pData, &m_data, sizeof(T));
        core::get_device_context()->Unmap(m_buffer, 0);

        if constexpr (Shader == shader_vertex)
        {
            core::get_device_context()->VSSetConstantBuffers(m_position, 1, &m_buffer);
        } else if constexpr (Shader == shader_pixel)
        {
            core::get_device_context()->PSSetConstantBuffers(m_position, 1, &m_buffer);
        }

        return true;
    }

    T& data() { return m_data; }

private:
    T             m_data{};
    u32           m_position{};
    ID3D11Buffer* m_buffer{};
};

} // namespace yae::gfx