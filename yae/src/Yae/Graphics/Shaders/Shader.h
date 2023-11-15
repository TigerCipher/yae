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

namespace yae::gfx
{


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

class shader
{
public:
    using parameter = std::function<void()>;

    shader()          = default;
    virtual ~shader() = default;

    virtual bool init(const wchar_t* vs_filename, const wchar_t* ps_filename, const char* vs_func_name, const char* ps_func_name,
                      const shader_layout& layout);
    virtual void shutdown();
    bool         render(u32 index_count, const math::matrix& view, ID3D11ShaderResourceView* texture) const;

    //void set_parameters(const std::initializer_list<parameter> params) { m_parameters = params; }

protected:
    bool set_parameters(const math::matrix& view, ID3D11ShaderResourceView* texture) const;

    struct matrix_buffer
    {
        math::matrix world;
        math::matrix view;
        math::matrix projection;
    };

    ID3D11VertexShader* m_vertex_shader{};
    ID3D11PixelShader*  m_pixel_shader{};
    ID3D11InputLayout*  m_layout{};
    ID3D11Buffer*       m_matrix_buffer{};
    ID3D11SamplerState* m_sampler_state{};

    std::vector<parameter> m_parameters{};
};


template<typename T>
concept shader_type = is_subclass<T, shader>;

template<shader_type T>
T* create_shader(const std::string& name, const shader_layout& layout)
{
    LOG_INFO("Loading {} shader", name);
    const std::string  vs_func = std::format("{}VertexShader", name);
    const std::string  ps_func = std::format("{}PixelShader", name);
    const std::wstring vs_file = std::format(L"../Shaders/{}.vs", std::wstring{ name.begin(), name.end() });
    const std::wstring ps_file = std::format(L"../Shaders/{}.ps", std::wstring{ name.begin(), name.end() });

    T* ret = new T;
    if (!ret->init(vs_file.c_str(), ps_file.c_str(), vs_func.c_str(), ps_func.c_str(), layout))
    {
        delete ret;
        return nullptr;
    }

    LOG_INFO("Done.");
    return ret;
}

} // namespace yae::gfx