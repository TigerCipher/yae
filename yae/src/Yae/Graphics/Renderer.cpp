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

ID3D11SamplerState* sampler_state{};
} // anonymous namespace

//void render3d(const model* model, const texture* tex, const math::matrix& world)
//{
//    if (!light_created)
//    {
//        light_pos               = { 3.f, 1.f, 3.f };
//        to_shader.diffuse_color = { 0.f, 1.f, 1.f, 1.f };
//        //to_shader.constant_factor  = 1.f;
//        //to_shader.linear_factor    = 0.09f;
//        //to_shader.quadratic_factor = 0.032f;
//        to_shader.radius    = 20.f;
//        to_shader.intensity = 4.5f;
//        to_shader.falloff   = 15.f;
//
//        light_created = true;
//    }
//    vertex_shader* vs = shaders::light_shader()->vs;
//    pixel_shader*  ps = shaders::light_shader()->ps;
//
//
//    const auto w = XMMatrixTranspose(world);
//    const auto p = XMMatrixTranspose(core::get_projection_matrix());
//    const auto v = XMMatrixTranspose(app::instance()->camera()->view());
//
//    vs->set_matrix("worldMatrix", w);
//    vs->set_matrix("projectionMatrix", p);
//    vs->set_matrix("viewMatrix", v);
//    vs->set_float3("cameraPosition", app::instance()->camera()->position());
//    vs->set_float3("lightPosition", light_pos);
//    vs->copy_all_buffers();
//    vs->bind();
//
//
//    ps->set_shader_resource_view("shaderTexture", tex->texture_view());
//    ps->set_sampler_state("SampleType", tex->sampler_state());
//
//    ps->set_float4("ambientColor", { 0.15f, 0.15f, 0.15f, 1.0f });
//    ps->set_float4("diffuseColor", { 1.f, 1.f, 1.f, 1.f });
//    ps->set_float3("lightDirection", { 1.f, 0.f, 1.f });
//    ps->set_float("specularPower", 32.f);
//    ps->set_float4("specularColor", { 1.f, 1.f, 1.f, 1.f });
//    ps->set_data("pointLights", &to_shader, sizeof(pl));
//
//
//    ps->copy_all_buffers();
//    ps->bind();
//
//    model->bind();
//
//    core::get_device_context()->DrawIndexed(model->index_count(), 0, 0);
//}

void init_renderer()
{
    D3D11_SAMPLER_DESC sampler_desc = {};
    sampler_desc.AddressU           = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc.AddressV           = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc.AddressW           = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc.Filter             = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampler_desc.MaxAnisotropy      = 16;
    sampler_desc.MaxLOD             = D3D11_FLOAT32_MAX;
    core::get_device()->CreateSamplerState(&sampler_desc, &sampler_state);
}
void shutdown_renderer()
{
    core::release(sampler_state);
}

void render3d(const model* model, const texture* tex, const math::matrix& world)
{
    vertex_shader* vs = shaders::deferred()->vs;
    pixel_shader*  ps = shaders::deferred()->ps;

    const auto w = XMMatrixTranspose(world);
    const auto p = XMMatrixTranspose(core::get_projection_matrix());
    const auto v = XMMatrixTranspose(app::instance()->camera()->view());
    vs->set_matrix("worldMatrix", w);
    vs->set_matrix("projectionMatrix", p);
    vs->set_matrix("viewMatrix", v);

    vs->copy_all_buffers();
    vs->bind();

    ps->set_shader_resource_view("textureSRV", tex->texture_view());
    ps->set_sampler_state("Sampler", tex->sampler_state());

    //ps->set_float3("lightDirection", { 1.f, -1.f, 0.f });
    //ps->set_float3("cameraPos", app::instance()->camera()->position());
    //ps->set_float3("lightColor", { 1.f, 1.f, 1.f });

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

void render_directional_light()
{
    vertex_shader* vs = shaders::directional_light()->vs;
    pixel_shader*  ps = shaders::directional_light()->ps;

    vs->bind();

    ps->set_shader_resource_view("positionGB", core::position_gbuffer());
    ps->set_shader_resource_view("normalGB", core::normal_gbuffer());
    ps->set_shader_resource_view("diffuseGB", core::diffuse_gbuffer());
    ps->set_sampler_state("Sampler", sampler_state);

    ps->set_float3("ambientColor", { 0.15f, 0.15f, 0.15f });
    ps->set_float3("lightColor", { 1.f, 1.f, 1.f });
    ps->set_float3("lightDirection", { 1.f, 0.f, 1.f });
    ps->set_float3("cameraPos", app::instance()->camera()->position());

    ps->copy_all_buffers();
    ps->bind();

    ID3D11Buffer* nothing{};
    constexpr u32 stride = sizeof(vertex_position_normal_texture);
    u32 offset{};
    core::get_device_context()->IASetVertexBuffers(0, 1, &nothing, &stride, &offset);
    core::get_device_context()->IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);
    core::get_device_context()->Draw(3, 0);

}

void render_lights(const model* model, const math::vec3& pos, const math::matrix& world, const math::vec3& light_color)
{
    vertex_shader* vs = shaders::lighting()->vs;
    pixel_shader*  ps = shaders::lighting()->ps;

    //model->bind();

    //const auto w = XMMatrixTranspose(world);
    //const auto p = XMMatrixTranspose(core::get_projection_matrix());
    //const auto v = XMMatrixTranspose(app::instance()->camera()->view());
    //vs->set_matrix("worldMatrix", w);
    //vs->set_matrix("projectionMatrix", p);
    //vs->set_matrix("viewMatrix", v);

    //vs->copy_all_buffers();
    vs->bind();

    ps->set_shader_resource_view("positionGB", core::position_gbuffer());
    ps->set_shader_resource_view("normalGB", core::normal_gbuffer());
    ps->set_shader_resource_view("diffuseGB", core::diffuse_gbuffer());
    ps->set_sampler_state("Sampler", sampler_state);

    ps->set_float3("cameraPos", app::instance()->camera()->position());
    ps->set_float3("lightColor", light_color);
    ps->set_float3("lightPos", pos);
    ps->set_float("radius", 25.0f);
    ps->set_float("intensity", 1.5f);
    ps->set_float("falloff", 15.0f);

    ps->copy_all_buffers();
    ps->bind();

    //core::get_device_context()->DrawIndexed(model->index_count(), 0, 0);
    ID3D11Buffer* nothing{};
    constexpr u32 stride = sizeof(vertex_position_normal_texture);
    u32           offset{};
    core::get_device_context()->IASetVertexBuffers(0, 1, &nothing, &stride, &offset);
    core::get_device_context()->IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);
    core::get_device_context()->Draw(3, 0);
}

} // namespace yae::gfx