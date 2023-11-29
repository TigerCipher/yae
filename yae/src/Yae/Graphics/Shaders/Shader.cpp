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
#include "Yae/Core/Win32Header.h"

#include "Yae/Core/System.h"
#include "Yae/Core/Application.h"
#include "../D3D11Core.h"

#include <d3dcompiler.h>


using namespace DirectX;

namespace yae::gfx
{


///////////// BASE SHADER ///////////////////

shader::shader()
{
    m_device  = core::get_device();
    m_context = core::get_device_context();
}

shader::~shader()
{
    core::release(m_blob);
}

bool shader::load_from_file(const std::string& filename)
{
    LOG_INFO("loading shader {}", filename);
    std::wstring widefile;
    if (!utl::utf8_to_utf16(filename.c_str(), widefile))
    {
        LOG_ERROR("Failed to convert shader filename '{}' to wide string", filename);
        return false;
    }

    DX_CALL(D3DReadFileToBlob(widefile.c_str(), &m_blob));

    m_valid = create_shader(m_blob);
    if (!m_valid)
    {
        LOG_FATAL("Failed to validate shader '{}'", filename);
        return false;
    }

    ID3D11ShaderReflection* refl{};
    D3DReflect(m_blob->GetBufferPointer(), m_blob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**) &refl);

    D3D11_SHADER_DESC desc{};
    refl->GetDesc(&desc);

    m_buffer_count     = desc.ConstantBuffers;
    m_constant_buffers = new constant_buffer[m_buffer_count];

    u32 res_count = desc.BoundResources;
    LOG_DEBUG("Found {} bound resources", res_count);
    for (u32 i = 0; i < res_count; ++i)
    {
        D3D11_SHADER_INPUT_BIND_DESC res_desc{};
        refl->GetResourceBindingDesc(i, &res_desc);

        switch (res_desc.Type)
        {
        case D3D_SIT_TEXTURE:
        {
            LOG_DEBUG("Found shader resource '{}' at bind index: {}", res_desc.Name, res_desc.BindPoint);
            shader_resource* srv = new shader_resource{ (u32) m_shader_resources.size(), res_desc.BindPoint };
            m_texture_table.insert(std::pair<std::string, shader_resource*>{ res_desc.Name, srv });
            m_shader_resources.push_back(srv);
        }
        break;
        case D3D_SIT_SAMPLER:
        {
            LOG_DEBUG("Found sampler '{}' at bind index: {}", res_desc.Name, res_desc.BindPoint);
            sampler* samp = new sampler{ (u32) m_samplers.size(), res_desc.BindPoint };
            m_sampler_table.insert(std::pair<std::string, sampler*>{ res_desc.Name, samp });
            m_samplers.push_back(samp);
        }
        break;
        default: LOG_WARN("Unhandled shader resource type `{}` found in `{}`", res_desc.Name, filename); break;
        }
    }

    for (u32 i = 0; i < m_buffer_count; ++i)
    {
        ID3D11ShaderReflectionConstantBuffer* cb = refl->GetConstantBufferByIndex(i);

        D3D11_SHADER_BUFFER_DESC buf_desc{};
        cb->GetDesc(&buf_desc);

        D3D11_SHADER_INPUT_BIND_DESC bind_desc{};
        refl->GetResourceBindingDescByName(buf_desc.Name, &bind_desc);

        m_constant_buffers[i].bind_index = bind_desc.BindPoint;
        m_constant_buffers[i].name       = buf_desc.Name;
        LOG_DEBUG("Found constant buffer '{}' at bind index: {}", buf_desc.Name, bind_desc.BindPoint);
        m_const_buffer_table.insert(std::pair<std::string, constant_buffer*>{ buf_desc.Name, &m_constant_buffers[i] });

        D3D11_BUFFER_DESC new_buf_desc{};
        new_buf_desc.Usage               = D3D11_USAGE_DEFAULT;
        new_buf_desc.ByteWidth           = buf_desc.Size;
        new_buf_desc.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
        new_buf_desc.CPUAccessFlags      = 0;
        new_buf_desc.MiscFlags           = 0;
        new_buf_desc.StructureByteStride = 0;

        DX_CALL(m_device->CreateBuffer(&new_buf_desc, nullptr, &m_constant_buffers[i].const_buffer));

        m_constant_buffers[i].size        = buf_desc.Size;
        m_constant_buffers[i].data_buffer = new u8[buf_desc.Size];
        ZeroMemory(m_constant_buffers[i].data_buffer, buf_desc.Size);

        for (u32 v = 0; v < buf_desc.Variables; ++v)
        {
            ID3D11ShaderReflectionVariable* var = cb->GetVariableByIndex(v);

            D3D11_SHADER_VARIABLE_DESC var_desc{};
            var->GetDesc(&var_desc);

            shader_variable sv{ var_desc.StartOffset, var_desc.Size, i };

            std::string name = var_desc.Name;
            LOG_DEBUG("Found variable '{}'", name);
            m_var_table.insert({ name, sv });
            m_constant_buffers[i].variables.push_back(sv);
        }
    }

    refl->Release();

    LOG_INFO("Done.");
    return true;
}

void shader::bind()
{
    if (!m_valid)
    {
        return;
    }

    set_shader_constant_buffers();
}

void shader::copy_all_buffers() const
{
    if (!m_valid)
    {
        return;
    }

    for (u32 i = 0; i < m_buffer_count; ++i)
    {
        m_context->UpdateSubresource(m_constant_buffers[i].const_buffer, 0, nullptr, m_constant_buffers[i].data_buffer, 0, 0);
    }
}

void shader::copy_buffer(u32 index) const
{
    if (!m_valid)
    {
        return;
    }

    if (index >= m_buffer_count)
    {
        return;
    }

    const constant_buffer* cb = &m_constant_buffers[index];
    if (!cb)
    {
        return;
    }

    m_context->UpdateSubresource(cb->const_buffer, 0, nullptr, cb->data_buffer, 0, 0);
}

void shader::copy_buffer(const std::string& name)
{
    if (!m_valid)
    {
        return;
    }

    const constant_buffer* cb = find_constant_buffer(name);
    if (!cb)
    {
        return;
    }

    m_context->UpdateSubresource(cb->const_buffer, 0, nullptr, cb->data_buffer, 0, 0);
}

bool shader::set_data(const std::string& name, const void* data, u32 size)
{
    shader_variable* var = find_variable(name, size);
    if (!var)
    {
        return false;
    }

    CopyMemory(m_constant_buffers[var->constant_buffer_index].data_buffer + var->byte_offset, data, size);
    return true;
}

bool shader::set_int(const std::string& name, i32 data)
{
    return set_data(name, &data, sizeof(i32));
}

bool shader::set_float(const std::string& name, f32 data)
{
    return set_data(name, &data, sizeof(f32));
}

bool shader::set_float2(const std::string& name, const f32 data[2])
{
    return set_data(name, &data, sizeof(f32) * 2);
}

bool shader::set_float2(const std::string& name, const math::vec2& data)
{
    return set_data(name, &data, sizeof(f32) * 2);
}
bool shader::set_float3(const std::string& name, const f32 data[3])
{
    return set_data(name, &data, sizeof(f32) * 3);
}
bool shader::set_float3(const std::string& name, const math::vec3& data)
{
    return set_data(name, &data, sizeof(f32) * 3);
}
bool shader::set_float4(const std::string& name, const f32 data[4])
{
    return set_data(name, &data, sizeof(f32) * 4);
}
bool shader::set_float4(const std::string& name, const math::vec4& data)
{
    return set_data(name, &data, sizeof(f32) * 4);
}
bool shader::set_matrix(const std::string& name, const f32 data[16])
{
    return set_data(name, &data, sizeof(f32) * 16);
}
bool shader::set_matrix(const std::string& name, const math::matrix& data)
{
    return set_data(name, &data, sizeof(f32) * 16);
}
bool shader::set_matrix(const std::string& name, const math::mat4& data)
{
    return set_data(name, &data, sizeof(f32) * 16);
}

const shader_variable* shader::variable_info(const std::string& name)
{
    return find_variable(name, 0);
}

const shader_resource* shader::srv_info(const std::string& name)
{
    const auto it = m_texture_table.find(name);
    if (it == m_texture_table.end())
    {
        return nullptr;
    }

    return it->second;
}

const shader_resource* shader::srv_info(u32 index) const
{
    assert(index < m_shader_resources.size());
    if (index >= m_shader_resources.size())
    {
        return nullptr;
    }

    return m_shader_resources[index];
}

const sampler* shader::sampler_info(const std::string& name)
{
    const auto it = m_sampler_table.find(name);
    if (it == m_sampler_table.end())
    {
        return nullptr;
    }

    return it->second;
}

const sampler* shader::sampler_info(u32 index) const
{
    assert(index < m_samplers.size());
    if (index >= m_samplers.size())
    {
        return nullptr;
    }

    return m_samplers[index];
}

u32 shader::buffer_size(u32 index)
{
    assert(index < m_buffer_count);
    if (index >= m_buffer_count)
    {
        return 0;
    }

    return m_constant_buffers[index].size;
}

const constant_buffer* shader::buffer_info(const std::string& name)
{
    return find_constant_buffer(name);
}

const constant_buffer* shader::buffer_info(u32 index)
{
    assert(index < m_buffer_count);
    if (index >= m_buffer_count)
    {
        return nullptr;
    }

    return &m_constant_buffers[index];
}

void shader::shutdown()
{
    for (u32 i = 0; i < m_buffer_count; ++i)
    {
        core::release(m_constant_buffers[i].const_buffer);
        delete[] m_constant_buffers[i].data_buffer;
    }

    if (m_constant_buffers)
    {
        delete[] m_constant_buffers;
        m_constant_buffers = nullptr;
    }

    for (u32 i = 0; i < m_shader_resources.size(); ++i)
    {
        delete m_shader_resources[i];
    }

    for (u32 i = 0; i < m_samplers.size(); ++i)
    {
        delete m_samplers[i];
    }

    m_var_table.clear();
    m_const_buffer_table.clear();
    m_sampler_table.clear();
    m_texture_table.clear();
}

shader_variable* shader::find_variable(const std::string& name, u32 size)
{
    const auto it = m_var_table.find(name);
    if (it == m_var_table.end())
    {
        return nullptr;
    }

    shader_variable* var = &it->second;

    // Validation of size
    if (size > 0 /*bypass*/ && var->size != size)
    {
        return nullptr;
    }

    return var;
}

constant_buffer* shader::find_constant_buffer(const std::string& name)
{
    const auto it = m_const_buffer_table.find(name);
    if (it == m_const_buffer_table.end())
    {
        return nullptr;
    }

    return it->second;
}


///////////// VERTEX SHADER ///////////////////

vertex_shader::~vertex_shader()
{
    shutdown();
}

bool vertex_shader::set_sampler_state(const std::string& name, ID3D11SamplerState* samp)
{
    const sampler* info = sampler_info(name);
    if (!info)
    {
        return false;
    }

    m_context->VSSetSamplers(info->bind_index, 1, &samp);
    return true;
}

bool vertex_shader::set_shader_resource_view(const std::string& name, ID3D11ShaderResourceView* srv)
{
    const shader_resource* info = srv_info(name);
    if (!info)
    {
        return false;
    }

    m_context->VSSetShaderResources(info->bind_index, 1, &srv);
    return true;
}

bool vertex_shader::create_shader(ID3D10Blob* blob)
{
    shutdown();
    LOG_DEBUG("Creating vertex shader");
    DX_CALL(m_device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &m_shader));

    if (m_input_layout)
    {
        return true;
    }

    ID3D11ShaderReflection* refl;
    D3DReflect(blob->GetBufferPointer(), blob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**) &refl);

    D3D11_SHADER_DESC desc{};
    refl->GetDesc(&desc);

    std::vector<D3D11_INPUT_ELEMENT_DESC> layout_desc{};
    LOG_DEBUG("Found {} layout input parameters", desc.InputParameters);
    for (u32 i = 0; i < desc.InputParameters; ++i)
    {
        D3D11_SIGNATURE_PARAMETER_DESC param_desc{};
        refl->GetInputParameterDesc(i, &param_desc);

        std::string per_instance_str = "_PER_INSTANCE";
        std::string sem              = param_desc.SemanticName;
        const i32   len_diff         = (i32)sem.size() - (i32)per_instance_str.size();
        bool        per_instance     = len_diff >= 0 && sem.compare(len_diff, per_instance_str.size(), per_instance_str) == 0;
        LOG_DEBUG("Found parameter '{}' - per instance? {}", sem, per_instance ? "Yes" : "No");

        D3D11_INPUT_ELEMENT_DESC elem_desc{};
        elem_desc.SemanticName         = param_desc.SemanticName;
        elem_desc.SemanticIndex        = param_desc.SemanticIndex;
        elem_desc.InputSlot            = 0;
        elem_desc.AlignedByteOffset    = D3D11_APPEND_ALIGNED_ELEMENT;
        elem_desc.InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
        elem_desc.InstanceDataStepRate = 0;

        if (per_instance)
        {
            elem_desc.InputSlot            = 1;
            elem_desc.InputSlotClass       = D3D11_INPUT_PER_INSTANCE_DATA;
            elem_desc.InstanceDataStepRate = 1;
            m_per_instance_compatible      = true;
        }

        if (param_desc.Mask == 1)
        {
            if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
                elem_desc.Format = DXGI_FORMAT_R32_UINT;
            else if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
                elem_desc.Format = DXGI_FORMAT_R32_SINT;
            else if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
                elem_desc.Format = DXGI_FORMAT_R32_FLOAT;
        } else if (param_desc.Mask <= 3)
        {
            if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
                elem_desc.Format = DXGI_FORMAT_R32G32_UINT;
            else if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
                elem_desc.Format = DXGI_FORMAT_R32G32_SINT;
            else if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
                elem_desc.Format = DXGI_FORMAT_R32G32_FLOAT;
        } else if (param_desc.Mask <= 7)
        {
            if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
                elem_desc.Format = DXGI_FORMAT_R32G32B32_UINT;
            else if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
                elem_desc.Format = DXGI_FORMAT_R32G32B32_SINT;
            else if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
                elem_desc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
        } else if (param_desc.Mask <= 15)
        {
            if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
                elem_desc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
            else if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
                elem_desc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
            else if (param_desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
                elem_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        }
        LOG_DEBUG("Parameter format: {}", (i32)elem_desc.Format);

        layout_desc.push_back(elem_desc);
    }

    DX_CALL(m_device->CreateInputLayout(layout_desc.data(), (u32)layout_desc.size(), blob->GetBufferPointer(), blob->GetBufferSize(),
                                        &m_input_layout));

    refl->Release();

    return true;
}

void vertex_shader::set_shader_constant_buffers()
{
    if (!m_valid)
    {
        return;
    }

    m_context->IASetInputLayout(m_input_layout);
    m_context->VSSetShader(m_shader, nullptr, 0);

    for (u32 i = 0; i < m_buffer_count; ++i)
    {
        m_context->VSSetConstantBuffers(m_constant_buffers[i].bind_index, 1, &m_constant_buffers[i].const_buffer);
    }
}

void vertex_shader::shutdown()
{
    shader::shutdown();
    core::release(m_shader);
    core::release(m_input_layout);
}


///////////// PIXEL SHADER ///////////////////


pixel_shader::~pixel_shader()
{
    shutdown();
}

bool pixel_shader::set_shader_resource_view(const std::string& name, ID3D11ShaderResourceView* srv)
{
    const shader_resource* info = srv_info(name);
    if (!info)
    {
        return false;
    }

    m_context->PSSetShaderResources(info->bind_index, 1, &srv);
    return true;
}

bool pixel_shader::set_sampler_state(const std::string& name, ID3D11SamplerState* samp)
{
    const sampler* info = sampler_info(name);
    if (!info)
    {
        return false;
    }

    m_context->PSSetSamplers(info->bind_index, 1, &samp);
    return true;
}

bool pixel_shader::create_shader(ID3D10Blob* blob)
{
    LOG_DEBUG("Creating pixel shader");
    shutdown();
    DX_CALL(m_device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &m_shader));

    return true;
}

void pixel_shader::set_shader_constant_buffers()
{
    if (!m_valid)
    {
        return;
    }

    m_context->PSSetShader(m_shader, nullptr, 0);

    for (u32 i = 0; i < m_buffer_count; ++i)
    {
        m_context->PSSetConstantBuffers(m_constant_buffers[i].bind_index, 1, &m_constant_buffers[i].const_buffer);
    }
}

void pixel_shader::shutdown()
{
    shader::shutdown();
    core::release(m_shader);
}


///////////// DOMAIN SHADER ///////////////////


domain_shader::~domain_shader()
{
    shutdown();
}

bool domain_shader::set_shader_resource_view(const std::string& name, ID3D11ShaderResourceView* srv)
{
    const shader_resource* info = srv_info(name);
    if (!info)
    {
        return false;
    }

    m_context->DSSetShaderResources(info->bind_index, 1, &srv);
    return true;
}

bool domain_shader::set_sampler_state(const std::string& name, ID3D11SamplerState* sampler_state)
{
    const sampler* info = sampler_info(name);
    if (!info)
    {
        return false;
    }

    m_context->DSSetSamplers(info->bind_index, 1, &sampler_state);
    return true;
}

bool domain_shader::create_shader(ID3D10Blob* blob)
{
    LOG_DEBUG("Creating domain shader");
    shutdown();
    DX_CALL(m_device->CreateDomainShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &m_shader));
    return true;
}

void domain_shader::set_shader_constant_buffers()
{
    if (!m_valid)
    {
        return;
    }

    m_context->DSSetShader(m_shader, nullptr, 0);

    for (u32 i = 0; i < m_buffer_count; ++i)
    {
        m_context->DSSetConstantBuffers(m_constant_buffers[i].bind_index, 1, &m_constant_buffers[i].const_buffer);
    }
}

void domain_shader::shutdown()
{
    shader::shutdown();
    core::release(m_shader);
}


///////////// HULL SHADER ///////////////////

hull_shader::~hull_shader()
{
    shutdown();
}

bool hull_shader::set_shader_resource_view(const std::string& name, ID3D11ShaderResourceView* srv)
{
    const shader_resource* info = srv_info(name);
    if (!info)
    {
        return false;
    }

    m_context->HSSetShaderResources(info->bind_index, 1, &srv);
    return true;
}

bool hull_shader::set_sampler_state(const std::string& name, ID3D11SamplerState* sampler_state)
{
    const sampler* info = sampler_info(name);
    if (!info)
    {
        return false;
    }

    m_context->HSSetSamplers(info->bind_index, 1, &sampler_state);
    return true;
}

bool hull_shader::create_shader(ID3D10Blob* blob)
{
    LOG_DEBUG("Creating hull shader");
    shutdown();
    DX_CALL(m_device->CreateHullShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &m_shader));
    return true;
}

void hull_shader::set_shader_constant_buffers()
{
    if (!m_valid)
    {
        return;
    }

    m_context->HSSetShader(m_shader, nullptr, 0);

    for (u32 i = 0; i < m_buffer_count; ++i)
    {
        m_context->HSSetConstantBuffers(m_constant_buffers[i].bind_index, 1, &m_constant_buffers[i].const_buffer);
    }
}

void hull_shader::shutdown()
{
    shader::shutdown();
    core::release(m_shader);
}


///////////// GEOMETRY SHADER ///////////////////

geometry_shader::~geometry_shader()
{
    shutdown();
}

bool geometry_shader::set_shader_resource_view(const std::string& name, ID3D11ShaderResourceView* srv)
{
    const shader_resource* info = srv_info(name);
    if (!info)
    {
        return false;
    }

    m_context->GSSetShaderResources(info->bind_index, 1, &srv);
    return true;
}

bool geometry_shader::set_sampler_state(const std::string& name, ID3D11SamplerState* sampler_state)
{
    const sampler* info = sampler_info(name);
    if (!info)
    {
        return false;
    }

    m_context->GSSetSamplers(info->bind_index, 1, &sampler_state);
    return true;
}

bool geometry_shader::create_compatible_stream_out_buffer(ID3D11Buffer** buffer, u32 vertex_count) const
{
    if (!m_use_stream_out || !m_valid || !m_stream_out_vertex_size)
    {
        return false;
    }

    D3D11_BUFFER_DESC desc;
    desc.BindFlags           = D3D11_BIND_STREAM_OUTPUT | D3D11_BIND_VERTEX_BUFFER;
    desc.ByteWidth           = m_stream_out_vertex_size * vertex_count;
    desc.CPUAccessFlags      = 0;
    desc.MiscFlags           = 0;
    desc.StructureByteStride = 0;
    desc.Usage               = D3D11_USAGE_DEFAULT;

    DX_CALL(m_device->CreateBuffer(&desc, nullptr, buffer));
    return true;
}

void geometry_shader::unbind_stream_out_stage()
{
    constexpr u32 offset{};
    ID3D11Buffer* unset[1]{};
    core::get_device_context()->SOSetTargets(1, unset, &offset);
}

bool geometry_shader::create_shader(ID3D10Blob* blob)
{
    LOG_DEBUG("Creating geometry shader");
    shutdown();

    if (m_use_stream_out)
    {
        return create_shader_stream_out(blob);
    }

    DX_CALL(m_device->CreateGeometryShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &m_shader));
    return true;
}

void geometry_shader::set_shader_constant_buffers()
{
    if (!m_valid)
    {
        return;
    }

    m_context->GSSetShader(m_shader, nullptr, 0);

    for (u32 i = 0; i < m_buffer_count; ++i)
    {
        m_context->GSSetConstantBuffers(m_constant_buffers[i].bind_index, 1, &m_constant_buffers[i].const_buffer);
    }
}

void geometry_shader::shutdown()
{
    shader::shutdown();
    core::release(m_shader);
}

bool geometry_shader::create_shader_stream_out(ID3D10Blob* blob)
{
    LOG_DEBUG("Creating geometry shader with stream out");
    shutdown();

    ID3D11ShaderReflection* refl;
    D3DReflect(blob->GetBufferPointer(), blob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**) &refl);

    D3D11_SHADER_DESC shader_desc{};
    refl->GetDesc(&shader_desc);

    m_stream_out_vertex_size = 0;
    std::vector<D3D11_SO_DECLARATION_ENTRY> sodecl{};
    for (u32 i = 0; i < shader_desc.OutputParameters; ++i)
    {
        D3D11_SIGNATURE_PARAMETER_DESC param_desc{};
        refl->GetOutputParameterDesc(i, &param_desc);

        D3D11_SO_DECLARATION_ENTRY entry{};
        entry.SemanticIndex  = param_desc.SemanticIndex;
        entry.SemanticName   = param_desc.SemanticName;
        entry.Stream         = param_desc.Stream;
        entry.StartComponent = 0;
        entry.OutputSlot     = 0;
        entry.ComponentCount = calculate_component_count(param_desc.Mask);

        m_stream_out_vertex_size += entry.ComponentCount * sizeof(f32);

        sodecl.push_back(entry);
    }

    const u32 rast = m_allow_stream_out_rasterization ? 0 : D3D11_SO_NO_RASTERIZED_STREAM;

    DX_CALL(m_device->CreateGeometryShaderWithStreamOutput(blob->GetBufferPointer(), blob->GetBufferSize(), sodecl.data(),
                                                           (u32)sodecl.size(), nullptr, 0, rast, nullptr, &m_shader));

    return true;
}

u32 geometry_shader::calculate_component_count(u32 mask)
{
    u32 result{};
    result += (u32) ((mask & 1) == 1);
    result += (u32) ((mask & 2) == 2);
    result += (u32) ((mask & 4) == 4);
    result += (u32) ((mask & 8) == 8);

    return result;
}


///////////// COMPUTE SHADER ///////////////////

compute_shader::~compute_shader()
{
    shutdown();
}

bool compute_shader::set_shader_resource_view(const std::string& name, ID3D11ShaderResourceView* srv)
{
    const shader_resource* info = srv_info(name);
    if (!info)
    {
        return false;
    }

    m_context->CSSetShaderResources(info->bind_index, 1, &srv);
    return true;
}

bool compute_shader::set_sampler_state(const std::string& name, ID3D11SamplerState* sampler_state)
{
    const sampler* info = sampler_info(name);
    if (!info)
    {
        return false;
    }

    m_context->CSSetSamplers(info->bind_index, 1, &sampler_state);
    return true;
}

void compute_shader::dispatch_by_groups(u32 groups_x, u32 groups_y, u32 groups_z) const
{
    m_context->Dispatch(groups_x, groups_y, groups_z);
}

void compute_shader::dispatch_by_threads(u32 threads_x, u32 threads_y, u32 threads_z) const
{
    m_context->Dispatch(std::max((u32) ceil((f32) threads_x / m_threads_x), 1u),
                        std::max((u32) ceil((f32) threads_y / m_threads_y), 1u),
                        std::max((u32) ceil((f32) threads_z / m_threads_z), 1u));
}

bool compute_shader::set_unordered_access_view(const std::string& name, ID3D11UnorderedAccessView* uav, u32 append_consume_offset)
{
    const u32 bind_index = unordered_access_view_index(name);
    if (bind_index == invalid_u32)
    {
        return false;
    }

    m_context->CSSetUnorderedAccessViews(bind_index, 1, &uav, &append_consume_offset);
    return true;
}

u32 compute_shader::unordered_access_view_index(const std::string& name)
{
    const auto it = m_uav_table.find(name);
    if (it == m_uav_table.end())
    {
        return invalid_u32;
    }

    return it->second;
}

bool compute_shader::create_shader(ID3D10Blob* blob)
{
    LOG_DEBUG("Creating compute shader");
    shutdown();

    DX_CALL(m_device->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &m_shader));

    ID3D11ShaderReflection* refl;
    D3DReflect(blob->GetBufferPointer(), blob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**) &refl);

    D3D11_SHADER_DESC shader_desc{};
    refl->GetDesc(&shader_desc);

    m_total_threads = refl->GetThreadGroupSize(&m_threads_x, &m_threads_y, &m_threads_z);

    u32 res_count = shader_desc.BoundResources;
    LOG_DEBUG("Found {} bound resources", shader_desc.BoundResources);
    for (u32 i = 0; i < res_count; ++i)
    {
        D3D11_SHADER_INPUT_BIND_DESC res_desc{};
        refl->GetResourceBindingDesc(i, &res_desc);

        switch (res_desc.Type)
        {
        case D3D_SIT_UAV_APPEND_STRUCTURED:
        case D3D_SIT_UAV_CONSUME_STRUCTURED:
        case D3D_SIT_UAV_RWBYTEADDRESS:
        case D3D_SIT_UAV_RWSTRUCTURED:
        case D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER:
        case D3D_SIT_UAV_RWTYPED:
            m_uav_table.insert({ res_desc.Name, res_desc.BindPoint });
            LOG_DEBUG("Found '{}'", res_desc.Name);
            break;
        default: break;
        }
    }

    refl->Release();
    return true;
}

void compute_shader::set_shader_constant_buffers()
{
    if (!m_valid)
    {
        return;
    }

    m_context->CSSetShader(m_shader, nullptr, 0);

    for (u32 i = 0; i < m_buffer_count; ++i)
    {
        m_context->CSSetConstantBuffers(m_constant_buffers[i].bind_index, 1, &m_constant_buffers[i].const_buffer);
    }
}

void compute_shader::shutdown()
{
    shader::shutdown();
    core::release(m_shader);
    m_uav_table.clear();
}


} // namespace yae::gfx