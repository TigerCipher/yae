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
//  File Name: Renderer.cpp
//  Date File Created: 11/22/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#include "Renderer.h"

#include "Shaders/ShaderLibrary.h"
#include "Yae/Core/Application.h"
#include "Light.h"

namespace yae::gfx
{

namespace
{
//point_light light{};
bool light_created{};

//struct pl
//{
//    math::vec4 diffuse_color;
//    f32        constant_factor;
//    f32        linear_factor;
//    f32        quadratic_factor;
//    f32        padding{};
//};

struct pl
{
    math::vec4 diffuse_color;
    f32        radius;
    f32        intensity;
    f32        falloff;
};

pl         to_shader{};
math::vec3 light_pos{};
} // anonymous namespace

void render3d(const model* model, const texture* tex, const math::matrix& world)
{
    if (!light_created)
    {
        light_pos               = { 3.f, 1.f, 3.f };
        to_shader.diffuse_color = { 0.f, 1.f, 1.f, 1.f };
        //to_shader.constant_factor  = 1.f;
        //to_shader.linear_factor    = 0.09f;
        //to_shader.quadratic_factor = 0.032f;
        to_shader.radius    = 20.f;
        to_shader.intensity = 4.5f;
        to_shader.falloff   = 15.f;

        light_created = true;
    }
    vertex_shader* vs = shaders::light_shader()->vs;
    pixel_shader*  ps = shaders::light_shader()->ps;


    const auto w = XMMatrixTranspose(world);
    const auto p = XMMatrixTranspose(core::get_projection_matrix());
    const auto v = XMMatrixTranspose(app::instance()->camera()->view());

    vs->set_matrix("worldMatrix", w);
    vs->set_matrix("projectionMatrix", p);
    vs->set_matrix("viewMatrix", v);
    vs->set_float3("cameraPosition", app::instance()->camera()->position());
    vs->set_float3("lightPosition", light_pos);
    vs->copy_all_buffers();
    vs->bind();


    ps->set_shader_resource_view("shaderTexture", tex->texture_view());
    ps->set_sampler_state("SampleType", tex->sampler_state());

    ps->set_float4("ambientColor", { 0.15f, 0.15f, 0.15f, 1.0f });
    ps->set_float4("diffuseColor", { 1.f, 1.f, 1.f, 1.f });
    ps->set_float3("lightDirection", { 1.f, 0.f, 1.f });
    ps->set_float("specularPower", 32.f);
    ps->set_float4("specularColor", { 1.f, 1.f, 1.f, 1.f });
    ps->set_data("pointLights", &to_shader, sizeof(pl));


    ps->copy_all_buffers();
    ps->bind();

    model->bind();

    core::get_device_context()->DrawIndexed(model->index_count(), 0, 0);
}

void render2d(const model* model, const texture* tex, const math::matrix& world)
{
    vertex_shader* vs = shaders::texture_shader()->vs;
    pixel_shader*  ps = shaders::texture_shader()->ps;

    const auto w = XMMatrixTranspose(world);
    const auto p = XMMatrixTranspose(core::get_orthographic_matrix());
    const auto v = XMMatrixTranspose(default_view_matrix());

    vs->set_matrix("worldMatrix", w);
    vs->set_matrix("projectionMatrix", p);
    vs->set_matrix("viewMatrix", v);

    vs->copy_all_buffers();
    vs->bind();

    ps->set_sampler_state("SamplerType", tex->sampler_state());
    ps->set_shader_resource_view("shaderTexture", tex->texture_view());

    ps->copy_all_buffers();
    ps->bind();

    model->bind();
    core::get_device_context()->DrawIndexed(model->index_count(), 0, 0);
}
} // namespace yae::gfx