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
//  File Name: Shader.cpp
//  Date File Created: 11/12/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------
#include "Shader.h"

#include "Yae/Core/System.h"
#include "../D3D11Core.h"

#include <fstream>
#include <d3dcompiler.h>


using namespace DirectX;

namespace yae::gfx
{

namespace
{
void output_error_message(ID3D10Blob* err_msg, const wchar_t* filename)
{
    const char* compile_errors = (char*) err_msg->GetBufferPointer();
    const u64   buffer_size    = err_msg->GetBufferSize();

    std::ofstream fout{ "shader-error.txt" };

    OutputDebugStringA("SHADER ERROR:\n");
    OutputDebugStringA(compile_errors);
    OutputDebugStringA("\n");

    for (u32 i = 0; i < buffer_size; ++i)
    {
        fout << compile_errors[i];
    }

    fout.close();

    core::release(err_msg);

    MessageBox(system::handle(), L"Error compiling shader. See shader-error.txt or debug console", filename, MB_OK);
}
} // anonymous namespace

bool shader::init(const wchar_t* vs_filename, const wchar_t* ps_filename, const char* vs_func_name, const char* ps_func_name,
                  const shader_layout& layout)
{
    ID3D10Blob* err_msg{};
    ID3D10Blob* vs_buffer{};
    ID3D10Blob* ps_buffer{};
    //D3D11_INPUT_ELEMENT_DESC polygon_layout[2];
    u32               num_elements{};
    D3D11_BUFFER_DESC matrix_buffer_desc{};

    LOG_INFO("Compiling vertex shader");
    if (FAILED(D3DCompileFromFile(vs_filename, nullptr, nullptr, vs_func_name, "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
                                  &vs_buffer, &err_msg)))
    {
        if (err_msg)
        {
            output_error_message(err_msg, vs_filename);
        } else
        {
            popup::show(vs_filename, L"Missing shader file", popup::style::error);
        }
        return false;
    }
    LOG_INFO("Done.");

    LOG_INFO("Compiling pixel shader");
    if (FAILED(D3DCompileFromFile(ps_filename, nullptr, nullptr, ps_func_name, "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
                                  &ps_buffer, &err_msg)))
    {
        if (err_msg)
        {
            output_error_message(err_msg, ps_filename);
        } else
        {
            popup::show(ps_filename, L"Missing shader file", popup::style::error);
        }
        return false;
    }
    LOG_INFO("Done.");

    DX_CALL(core::get_device()->CreateVertexShader(vs_buffer->GetBufferPointer(), vs_buffer->GetBufferSize(), nullptr,
                                                   &m_vertex_shader));
    DX_CALL(core::get_device()->CreatePixelShader(ps_buffer->GetBufferPointer(), ps_buffer->GetBufferSize(), nullptr,
                                                  &m_pixel_shader));

    //setup_polygon(polygon_layout);

    //num_elements = std::size(polygon_layout);
    num_elements = layout.size();
    DX_CALL(core::get_device()->CreateInputLayout(layout.data(), num_elements, vs_buffer->GetBufferPointer(),
                                                  vs_buffer->GetBufferSize(), &m_layout));

    core::release(vs_buffer);
    core::release(ps_buffer);

    // Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
    matrix_buffer_desc.Usage               = D3D11_USAGE_DYNAMIC;
    matrix_buffer_desc.ByteWidth           = sizeof(matrix_buffer);
    matrix_buffer_desc.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
    matrix_buffer_desc.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
    matrix_buffer_desc.MiscFlags           = 0;
    matrix_buffer_desc.StructureByteStride = 0;

    DX_CALL(core::get_device()->CreateBuffer(&matrix_buffer_desc, nullptr, &m_matrix_buffer));

    D3D11_SAMPLER_DESC sampler_desc;
    sampler_desc.Filter         = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampler_desc.AddressU       = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc.AddressV       = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc.AddressW       = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc.MipLODBias     = 0.f;
    sampler_desc.MaxAnisotropy  = 1;
    sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    sampler_desc.BorderColor[0] = 0.f;
    sampler_desc.BorderColor[1] = 0.f;
    sampler_desc.BorderColor[2] = 0.f;
    sampler_desc.BorderColor[3] = 0.f;
    sampler_desc.MinLOD         = 0.f;
    sampler_desc.MaxLOD         = D3D11_FLOAT32_MAX;

    DX_CALL(core::get_device()->CreateSamplerState(&sampler_desc, &m_sampler_state));

    return true;
}

void shader::shutdown()
{
    core::release(m_sampler_state);
    core::release(m_matrix_buffer);
    core::release(m_layout);
    core::release(m_pixel_shader);
    core::release(m_vertex_shader);
}

bool shader::render(u32 index_count, const math::matrix& view, ID3D11ShaderResourceView* texture) const
{
    if (!set_parameters(view, texture))
    {
        return false;
    }


    core::get_device_context()->IASetInputLayout(m_layout);
    core::get_device_context()->VSSetShader(m_vertex_shader, nullptr, 0);
    core::get_device_context()->PSSetShader(m_pixel_shader, nullptr, 0);
    core::get_device_context()->PSSetSamplers(0, 1, &m_sampler_state);
    core::get_device_context()->DrawIndexed(index_count, 0, 0);

    return true;
}

// TODO find a better way to make shaders more abstracted, HUD/UI shaders won't need lighting for instance
bool shader::set_parameters(const math::matrix& view_, ID3D11ShaderResourceView* texture) const
{
    D3D11_MAPPED_SUBRESOURCE mapped_res{};
    matrix_buffer*           data_ptr{};
    u32                      buffer_num{};
    core::get_device_context()->PSSetShaderResources(0, 1, &texture);
    const auto world = XMMatrixTranspose(core::get_world_matrix());
    const auto view  = XMMatrixTranspose(view_);
    const auto proj  = XMMatrixTranspose(core::get_projection_matrix());

    DX_CALL(core::get_device_context()->Map(m_matrix_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_res));
    data_ptr             = (matrix_buffer*) mapped_res.pData;
    data_ptr->world      = world;
    data_ptr->view       = view;
    data_ptr->projection = proj;

    core::get_device_context()->Unmap(m_matrix_buffer, 0);

    buffer_num = 0;
    core::get_device_context()->VSSetConstantBuffers(buffer_num, 1, &m_matrix_buffer);

    

    // TODO: Base shader to end here? as basic texture shader?
    //for (auto p : m_parameters)
    //{
    //    p();
    //}

    return true;
}
} // namespace yae::gfx