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
//  File Name: LightShader.h
//  Date File Created: 11/15/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#pragma once
#include "Shader.h"
#include "../Light.h"

namespace yae::gfx
{

class light_shader : public shader
{
public:
    ~light_shader() override = default;
    bool init(const wchar_t* vs_filename, const wchar_t* ps_filename, const char* vs_func_name, const char* ps_func_name,
              const shader_layout& layout) override;
    void shutdown() override;
    bool render(u32 index_count, const math::matrix& view, ID3D11ShaderResourceView* texture, const directional_light& light) const;

protected:
    struct light_buffer
    {
        math::vec4 diffuse_color;
        math::vec3 direction;
        f32        padding{};
    };

    ID3D11Buffer* m_light_buffer{};
};

}