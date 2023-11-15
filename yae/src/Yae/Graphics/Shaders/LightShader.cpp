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
//  File Name: LightShader.cpp
//  Date File Created: 11/15/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------
#include "LightShader.h"
#include "../D3D11Core.h"

namespace yae::gfx
{
bool light_shader::init(const wchar_t* vs_filename, const wchar_t* ps_filename, const char* vs_func_name,
                        const char* ps_func_name, const shader_layout& layout)
{
    if (!shader::init(vs_filename, ps_filename, vs_func_name, ps_func_name, layout))
    {
        return false;
    }

    D3D11_BUFFER_DESC light_desc{};
    light_desc.Usage               = D3D11_USAGE_DYNAMIC;
    light_desc.ByteWidth           = sizeof(light_buffer);
    light_desc.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
    light_desc.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
    light_desc.MiscFlags           = 0;
    light_desc.StructureByteStride = 0;

    DX_CALL(core::get_device()->CreateBuffer(&light_desc, nullptr, &m_light_buffer));

    D3D11_BUFFER_DESC camera_desc{};
    camera_desc.Usage               = D3D11_USAGE_DYNAMIC;
    camera_desc.ByteWidth           = sizeof(camera_buffer);
    camera_desc.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
    camera_desc.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
    camera_desc.MiscFlags           = 0;
    camera_desc.StructureByteStride = 0;

    DX_CALL(core::get_device()->CreateBuffer(&camera_desc, nullptr, &m_camera_buffer));

    return true;
}
void light_shader::shutdown()
{
    core::release(m_camera_buffer);
    core::release(m_light_buffer);
    shader::shutdown();
}

bool light_shader::render(u32 index_count, const camera* cam, ID3D11ShaderResourceView* texture, const base_light& light)
{
    D3D11_MAPPED_SUBRESOURCE mapped_res{};

    DX_CALL(core::get_device_context()->Map(m_camera_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_res));
    auto* cam_ptr     = (camera_buffer*) mapped_res.pData;
    cam_ptr->position = cam->position();
    core::get_device_context()->Unmap(m_camera_buffer, 0);
    m_buffer.vs(&m_camera_buffer);

    DX_CALL(core::get_device_context()->Map(m_light_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_res));
    auto* light_ptr           = (light_buffer*) mapped_res.pData;
    light_ptr->ambient_color  = light.ambient_color;
    light_ptr->diffuse_color  = light.diffuse_color;
    light_ptr->direction      = light.direction;
    light_ptr->specular_power = light.specular_power;
    light_ptr->specular_color = light.specular_color;
    core::get_device_context()->Unmap(m_light_buffer, 0);
    m_buffer.ps(&m_light_buffer);

    return shader::render(index_count, cam, texture);
}

} // namespace yae::gfx