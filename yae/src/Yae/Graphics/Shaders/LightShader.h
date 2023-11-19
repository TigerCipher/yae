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

    void set_light(base_light* light) { m_light = light; }
    void set_point_lights(point_light lights[4]) { m_point_lights = lights; }

protected:
    bool set_parameters(const math::matrix& world) override;

    base_light*                                               m_light{};
    point_light*                                              m_point_lights{};
    constant_buffer<cb::light_buffer, shader_pixel>           m_light_buffer{ 0 };
    constant_buffer<cb::point_light_buffer, shader_pixel>     m_pl_buffer{ 1 };
    constant_buffer<cb::camera_buffer, shader_vertex>         m_camera_buffer{ 1 };
    constant_buffer<cb::light_position_buffer, shader_vertex> m_light_pos_buffer{ 2 };
};

} // namespace yae::gfx