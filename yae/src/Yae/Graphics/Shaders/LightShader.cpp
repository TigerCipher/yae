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

    if (!m_light_buffer.init())
    {
        LOG_ERROR("Failed to initialize light constant buffer");
        return false;
    }

    if (!m_camera_buffer.init())
    {
        LOG_ERROR("Failed to initialize camera constant buffer");
        return false;
    }

    return true;
}
void light_shader::shutdown()
{
    m_camera_buffer.release();
    m_light_buffer.release();
    shader::shutdown();
}
bool light_shader::set_parameters()
{
    m_camera_buffer.data().position = m_camera->position();

    if(!m_light)
    {
        LOG_ERROR("light_shader::set_light must be called before render");
        return false;
    }
    m_light_buffer.data().ambient_color  = m_light->ambient_color;
    m_light_buffer.data().diffuse_color  = m_light->diffuse_color;
    m_light_buffer.data().direction      = m_light->direction;
    m_light_buffer.data().specular_power = m_light->specular_power;
    m_light_buffer.data().specular_color = m_light->specular_color;


    if(!m_camera_buffer.apply())
    {
        return false;
    }

    if(!m_light_buffer.apply())
    {
        return false;
    }
    return shader::set_parameters();
}

} // namespace yae::gfx