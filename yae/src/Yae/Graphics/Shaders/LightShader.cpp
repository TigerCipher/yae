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

    D3D11_BUFFER_DESC desc{};
    desc.Usage               = D3D11_USAGE_DYNAMIC;
    desc.ByteWidth           = sizeof(light_buffer);
    desc.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags           = 0;
    desc.StructureByteStride = 0;

    DX_CALL(core::get_device()->CreateBuffer(&desc, nullptr, &m_light_buffer));

    return true;
}
void light_shader::shutdown()
{
    core::release(m_light_buffer);
    shader::shutdown();
}
bool light_shader::render(u32 index_count, const math::matrix& view, ID3D11ShaderResourceView* texture,
                          const directional_light& light)
{
    D3D11_MAPPED_SUBRESOURCE mapped_res{};
    DX_CALL(core::get_device_context()->Map(m_light_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_res));

    auto* light_ptr          = (light_buffer*) mapped_res.pData;
    light_ptr->diffuse_color = light.diffuse_color;
    light_ptr->direction     = light.direction;
    core::get_device_context()->Unmap(m_light_buffer, 0);

    constexpr u32 buffer_num = 0; // maybe a shader_parameter struct or class to keep track of VS/PS buffer nums?
    /*
     *class shader_parameter{
     *public:
     *void vs(u32 num, ID3D11Buffer* const* buffers) { core::get_device_context()->VSSetConstantBuffers(m_vs_buffer_num++, num, buffers); }
     *void ps(u32 num, ID3D11Buffer* const* buffers) { core::get_device_context()->PSSetConstantBuffers(m_ps_buffer_num++, num, buffers); }
     *};
     *
     *m_shader_param.ps(1, &m_light_buffer);
     */
    m_buffer.ps(&m_light_buffer);
    //core::get_device_context()->PSSetConstantBuffers(buffer_num, 1, &m_light_buffer);

    return shader::render(index_count, view, texture);
}

} // namespace yae::gfx