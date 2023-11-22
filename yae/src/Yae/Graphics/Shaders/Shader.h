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
//  File Name: Shader.h
//  Date File Created: 11/12/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#pragma once
#include "../D3D11Common.h"

#include <functional>
#include <format>

//namespace yae
//{
//class game_object;
//}

namespace yae::gfx
{

enum render_type
{
    render_2d,
    render_3d,
};

template<typename T>
constexpr DXGI_FORMAT resolve_shader_type() // by default, returns uint
{
    if constexpr (std::is_same_v<T, f32>)
    {
        return DXGI_FORMAT_R32_FLOAT;
    } else if constexpr (std::is_same_v<T, math::vec2>)
    {
        return DXGI_FORMAT_R32G32_FLOAT;
    } else if constexpr (std::is_same_v<T, math::vec3>)
    {
        return DXGI_FORMAT_R32G32B32_FLOAT;
    } else if constexpr (std::is_same_v<T, math::vec4>)
    {
        return DXGI_FORMAT_R32G32B32A32_FLOAT;
    }

    return DXGI_FORMAT_R32_UINT;
}

class shader_layout
{
public:
    shader_layout()  = default;
    ~shader_layout() = default;

    template<typename T>
    void add(const char* name, bool aligned = true)
    {
        if (m_elements.empty())
        {
            m_elements.push_back({});
            auto& elem                = m_elements.back();
            elem.SemanticName         = name;
            elem.SemanticIndex        = 0;
            elem.Format               = resolve_shader_type<T>();
            elem.InputSlot            = 0;
            elem.AlignedByteOffset    = 0;
            elem.InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
            elem.InstanceDataStepRate = 0;
        } else
        {
            m_elements.push_back({});
            auto& elem                = m_elements.back();
            elem.SemanticName         = name;
            elem.SemanticIndex        = 0;
            elem.Format               = resolve_shader_type<T>();
            elem.InputSlot            = 0;
            elem.AlignedByteOffset    = aligned ? D3D11_APPEND_ALIGNED_ELEMENT : 0;
            elem.InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
            elem.InstanceDataStepRate = 0;
        }
    }

    constexpr const D3D11_INPUT_ELEMENT_DESC* data() const { return m_elements.data(); }

    constexpr u32 size() const { return (u32) m_elements.size(); }

private:
    std::vector<D3D11_INPUT_ELEMENT_DESC> m_elements{};
};


struct shader_variable
{
    u32 byte_offset{};
    u32 size{};
    u32 constant_buffer_index{};
};

struct constant_buffer
{
    std::string                  name{};
    u32                          size{};
    u32                          bind_index{};
    ID3D11Buffer*                const_buffer{};
    u8*                          data_buffer{};
    std::vector<shader_variable> variables{};
};

struct shader_resource
{
    u32 index{};
    u32 bind_index{};
};

struct sampler
{
    u32 index{};
    u32 bind_index{};
};

class shader
{
public:
    shader();
    virtual ~shader();

    bool load_from_file(const std::string& filename);

    constexpr bool is_valid() const { return m_valid; }

    void bind();
    void copy_all_buffers() const;
    void copy_buffer(u32 index) const;
    void copy_buffer(const std::string& name);


    bool set_data(const std::string& name, const void* data, u32 size);

    bool set_int(const std::string& name, i32 data);
    bool set_float(const std::string& name, f32 data);
    bool set_float2(const std::string& name, const f32 data[2]);
    bool set_float2(const std::string& name, const math::vec2& data);
    bool set_float3(const std::string& name, const f32 data[3]);
    bool set_float3(const std::string& name, const math::vec3& data);
    bool set_float4(const std::string& name, const f32 data[4]);
    bool set_float4(const std::string& name, const math::vec4& data);
    bool set_matrix(const std::string& name, const f32 data[16]);
    bool set_matrix(const std::string& name, const math::matrix& data);
    bool set_matrix(const std::string& name, const math::mat4& data);

    virtual bool set_shader_resource_view(const std::string& name, ID3D11ShaderResourceView* srv) = 0;
    virtual bool set_sampler_state(const std::string& name, ID3D11SamplerState* sampler_state)    = 0;

    const shader_variable* variable_info(const std::string& name);

    const shader_resource* srv_info(const std::string& name);
    const shader_resource* srv_info(u32 index) const;
    u32                    srv_count() const { return (u32) m_texture_table.size(); }

    const sampler* sampler_info(const std::string& name);
    const sampler* sampler_info(u32 index) const;
    u32            sampler_count() const { return (u32) m_sampler_table.size(); }

    constexpr u32 buffer_count() const { return m_buffer_count; }
    u32           buffer_size(u32 index);

    const constant_buffer* buffer_info(const std::string& name);
    const constant_buffer* buffer_info(u32 index);

    constexpr ID3D10Blob* blob() const { return m_blob; }

protected:
    virtual bool create_shader(ID3D10Blob* blob) = 0;
    virtual void set_shader_constant_buffers()   = 0;
    virtual void shutdown();

    /**
     * \brief Finds a <code>shader_variable</code> by name
     * \param name The name of the variable
     * \param size The size of the variable for validation, or 0 to bypass validation
     * \return nullptr if the variable is either not found or not validated, a pointer to the variable if found
     */
    shader_variable* find_variable(const std::string& name, u32 size);

    /**
     * \brief Finds a <code>constant_buffer</code> by name
     * \param name The name of the constant buffer
     * \return A pointer to the constant buffer or nullptr if not found
     */
    constant_buffer* find_constant_buffer(const std::string& name);

    bool m_valid{};
    u32  m_buffer_count{};

    constant_buffer*                                  m_constant_buffers{};
    std::vector<shader_resource*>                     m_shader_resources{};
    std::vector<sampler*>                             m_samplers{};
    std::unordered_map<std::string, constant_buffer*> m_const_buffer_table{};
    std::unordered_map<std::string, shader_variable>  m_var_table{};
    std::unordered_map<std::string, shader_resource*> m_texture_table{};
    std::unordered_map<std::string, sampler*>         m_sampler_table{};

    ID3D10Blob*          m_blob{};
    ID3D11Device*        m_device{};
    ID3D11DeviceContext* m_context{};
};


class vertex_shader : public shader
{
public:
    vertex_shader() = default;
    vertex_shader(ID3D11InputLayout* layout, bool per_instance_compatible) :
        m_input_layout{ layout }, m_per_instance_compatible{ per_instance_compatible }
    {}
    ~vertex_shader() override;

    constexpr ID3D11VertexShader* get_shader() const { return m_shader; }
    constexpr ID3D11InputLayout*  layout() const { return m_input_layout; }

    constexpr bool per_instance_compatible() const { return m_per_instance_compatible; }

    bool set_sampler_state(const std::string& name, ID3D11SamplerState* sampler) override;
    bool set_shader_resource_view(const std::string& name, ID3D11ShaderResourceView* srv) override;

protected:
    bool create_shader(ID3D10Blob* blob) override;
    void set_shader_constant_buffers() override;
    void shutdown() override;

    ID3D11InputLayout*  m_input_layout{};
    bool                m_per_instance_compatible{};
    ID3D11VertexShader* m_shader{};
};

class pixel_shader : public shader
{
public:
    pixel_shader() = default;
    ~pixel_shader() override;

    constexpr ID3D11PixelShader* get_shader() const { return m_shader; }

    bool set_shader_resource_view(const std::string& name, ID3D11ShaderResourceView* srv) override;
    bool set_sampler_state(const std::string& name, ID3D11SamplerState* sampler) override;

protected:
    bool create_shader(ID3D10Blob* blob) override;
    void set_shader_constant_buffers() override;
    void shutdown() override;

    ID3D11PixelShader* m_shader{};
};

class domain_shader : public shader
{
public:
    domain_shader() = default;
    ~domain_shader() override;

    constexpr ID3D11DomainShader* get_shader() const { return m_shader; }

    bool set_shader_resource_view(const std::string& name, ID3D11ShaderResourceView* srv) override;
    bool set_sampler_state(const std::string& name, ID3D11SamplerState* sampler_state) override;

protected:
    bool create_shader(ID3D10Blob* blob) override;
    void set_shader_constant_buffers() override;
    void shutdown() override;

    ID3D11DomainShader* m_shader{};
};

class hull_shader : public shader
{
public:
    hull_shader() = default;
    ~hull_shader() override;

    constexpr ID3D11HullShader* get_shader() const { return m_shader; }

    bool set_shader_resource_view(const std::string& name, ID3D11ShaderResourceView* srv) override;
    bool set_sampler_state(const std::string& name, ID3D11SamplerState* sampler_state) override;

protected:
    bool create_shader(ID3D10Blob* blob) override;
    void set_shader_constant_buffers() override;
    void shutdown() override;

    ID3D11HullShader* m_shader{};
};

class geometry_shader : public shader
{
public:
    geometry_shader() = default;
    geometry_shader(bool use_stream_out, bool allow_stream_out_rasterization) :
        m_use_stream_out{ use_stream_out }, m_allow_stream_out_rasterization{ allow_stream_out_rasterization }
    {}
    ~geometry_shader() override;

    constexpr ID3D11GeometryShader* get_shader() const { return m_shader; }

    bool set_shader_resource_view(const std::string& name, ID3D11ShaderResourceView* srv) override;
    bool set_sampler_state(const std::string& name, ID3D11SamplerState* sampler_state) override;

    bool create_compatible_stream_out_buffer(ID3D11Buffer** buffer, u32 vertex_count) const;

    static void unbind_stream_out_stage();

protected:
    bool create_shader(ID3D10Blob* blob) override;
    void set_shader_constant_buffers() override;
    void shutdown() override;
    bool       create_shader_stream_out(ID3D10Blob* blob);
    static u32  calculate_component_count(u32 mask);


    ID3D11GeometryShader* m_shader{};
    bool                  m_use_stream_out{};
    bool                  m_allow_stream_out_rasterization{};
    u32                   m_stream_out_vertex_size{};
};

class compute_shader : public shader
{
public:
    compute_shader() = default;
    ~compute_shader() override;

    constexpr ID3D11ComputeShader* get_shader() const { return m_shader; }

    bool set_shader_resource_view(const std::string& name, ID3D11ShaderResourceView* srv) override;
    bool set_sampler_state(const std::string& name, ID3D11SamplerState* sampler_state) override;

    void dispatch_by_groups(u32 groups_x, u32 groups_y, u32 groups_z) const;
    void dispatch_by_threads(u32 threads_x, u32 threads_y, u32 threads_z) const;

    bool set_unordered_access_view(const std::string& name, ID3D11UnorderedAccessView* uav,
                                   u32 append_consume_offset = invalid_u32);

    u32 unordered_access_view_index(const std::string& name);

protected:
    bool create_shader(ID3D10Blob* blob) override;
    void set_shader_constant_buffers() override;
    void shutdown() override;

    ID3D11ComputeShader* m_shader{};
    u32                  m_threads_x{};
    u32                  m_threads_y{};
    u32                  m_threads_z{};
    u32                  m_total_threads{};

    std::unordered_map<std::string, u32> m_uav_table{};
};

} // namespace yae::gfx