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
//  File Name: ColorShader.cpp
//  Date File Created: 11/12/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------
#include "ColorShader.h"

#include "Yae/Core/System.h"
#include "../D3D11Core.h"

#include <d3dcompiler.h>
#include <fstream>


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

bool color_shader::init()
{
    wchar_t vs_filename[128];
    wchar_t ps_filename[128];

    i32 error = wcscpy_s(vs_filename, 128, L"../Shaders/color.vs");
    if (error)
    {
        return false;
    }

    error = wcscpy_s(ps_filename, 128, L"../Shaders/color.ps");
    if (error)
    {
        return false;
    }

    ID3D10Blob*              err_msg{};
    ID3D10Blob*              vs_buffer{};
    ID3D10Blob*              ps_buffer{};
    D3D11_INPUT_ELEMENT_DESC polygon_layout[2];
    u32                      num_elements{};
    D3D11_BUFFER_DESC        matrix_buffer_desc{};

    if (FAILED(D3DCompileFromFile(vs_filename, nullptr, nullptr, "ColorVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
                                  &vs_buffer, &err_msg)))
    {
        if (err_msg)
        {
            output_error_message(err_msg, vs_filename);
        } else
        {
            MessageBox(system::handle(), vs_filename, L"Missing shader file", MB_OK);
        }
        return false;
    }

    if (FAILED(D3DCompileFromFile(ps_filename, nullptr, nullptr, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
                                  &ps_buffer, &err_msg)))
    {
        if (err_msg)
        {
            output_error_message(err_msg, ps_filename);
        } else
        {
            MessageBox(system::handle(), ps_filename, L"Missing shader file", MB_OK);
        }
        return false;
    }

    DX_CALL(core::get_device()->CreateVertexShader(vs_buffer->GetBufferPointer(), vs_buffer->GetBufferSize(), nullptr,
                                                   &m_vertex_shader));
    DX_CALL(core::get_device()->CreatePixelShader(ps_buffer->GetBufferPointer(), ps_buffer->GetBufferSize(), nullptr,
                                                  &m_pixel_shader));

    // This setup needs to match the vertex structure in the model class and in the shader.
    polygon_layout[0].SemanticName         = "POSITION";
    polygon_layout[0].SemanticIndex        = 0;
    polygon_layout[0].Format               = DXGI_FORMAT_R32G32B32_FLOAT;
    polygon_layout[0].InputSlot            = 0;
    polygon_layout[0].AlignedByteOffset    = 0;
    polygon_layout[0].InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
    polygon_layout[0].InstanceDataStepRate = 0;

    polygon_layout[1].SemanticName         = "COLOR";
    polygon_layout[1].SemanticIndex        = 0;
    polygon_layout[1].Format               = DXGI_FORMAT_R32G32B32A32_FLOAT;
    polygon_layout[1].InputSlot            = 0;
    polygon_layout[1].AlignedByteOffset    = D3D11_APPEND_ALIGNED_ELEMENT;
    polygon_layout[1].InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
    polygon_layout[1].InstanceDataStepRate = 0;

    num_elements = std::size(polygon_layout);

    DX_CALL(core::get_device()->CreateInputLayout(polygon_layout, num_elements, vs_buffer->GetBufferPointer(),
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

    return true;
}

void color_shader::shutdown()
{
    core::release(m_matrix_buffer);
    core::release(m_layout);
    core::release(m_pixel_shader);
    core::release(m_vertex_shader);
}

bool color_shader::render(i32 index_count, const math::matrix& view) const
{
    if (!set_parameters(view))
    {
        return false;
    }

    core::get_device_context()->IASetInputLayout(m_layout);
    core::get_device_context()->VSSetShader(m_vertex_shader, nullptr, 0);
    core::get_device_context()->PSSetShader(m_pixel_shader, nullptr, 0);
    core::get_device_context()->DrawIndexed(index_count, 0, 0);

    return true;
}

bool color_shader::set_parameters(const math::matrix& view_) const
{
    D3D11_MAPPED_SUBRESOURCE mapped_res{};
    matrix_buffer*           data_ptr{};
    u32                      buffer_num{};

    const auto world = XMMatrixTranspose(core::get_world_matrix());
    const auto view  = XMMatrixTranspose(view_);
    const auto proj  = XMMatrixTranspose(core::get_projection_matrix());

    DX_CALL(core::get_device_context()->Map(m_matrix_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_res));
    data_ptr             = (matrix_buffer*) mapped_res.pData;
    data_ptr->world      = world;
    data_ptr->view       = view;
    data_ptr->projection = proj;

    core::get_device_context()->Unmap(m_matrix_buffer, 0);

    core::get_device_context()->VSSetConstantBuffers(buffer_num, 1, &m_matrix_buffer);

    return true;
}

} // namespace yae::gfx