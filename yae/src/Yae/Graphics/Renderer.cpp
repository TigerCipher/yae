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

std::vector<std::pair<game_object*, pointlight_component*>> pointlights{};

ID3D11SamplerState* sampler_state{};

math::vec4 ambient{0.95f, 0.95f, 0.95f, 1.f};
math::vec3 light_direction{1.f, 0.f, -1.f};
} // anonymous namespace


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

void render3d(const model* model, const math::matrix& world, const material& mat)
{
    ID3D11SamplerState* sampler{};
    if(!mat.sampler)
    {
        sampler = default_sampler_state();
    }else
    {
        sampler = mat.sampler;
    }
    vertex_shader* vs = shaders::deferred()->vs;
    pixel_shader*  ps;
    if (!mat.blend)
    {
        ps = shaders::deferred()->ps;
    } else
    {
        ps = shaders::multitexture()->ps;
    }

    const auto w = XMMatrixTranspose(world);
    const auto p = XMMatrixTranspose(core::get_projection_matrix());
    const auto v = XMMatrixTranspose(app::instance()->camera()->view());
    vs->set_matrix("worldMatrix", w);
    vs->set_matrix("projectionMatrix", p);
    vs->set_matrix("viewMatrix", v);

    vs->copy_all_buffers();
    vs->bind();

    if (mat.diffuse)
    {
        ps->set_shader_resource_view("textureSRV", mat.diffuse);
    } else
    {
        ps->set_shader_resource_view("textureSRV", assets::load_texture("./assets/textures/default.tga")->texture_view());
    }
    if (mat.blend && ps == shaders::multitexture()->ps)
    {
        ps->set_shader_resource_view("textureSRVBlend", mat.blend);
    }

    if(mat.normal)
    {
        ps->set_shader_resource_view("textureSRVBump", mat.normal);
    }else
    {
        ps->set_shader_resource_view("textureSRVBump", assets::load_texture("./assets/textures/default_normal.tga")->texture_view());
    }

    ps->set_sampler_state("Sampler", sampler);

    ps->set_float4("tintColor", mat.tint);
    ps->set_float4("ambientColor", ambient);
    ps->set_float3("lightDirection", light_direction);

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

    // TODO: Temp
    ps->set_float4("tintColor", { 1.f, 1.f, 1.f, 1.f });

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

    ps->set_float3("ambientColor", { 0.05f, 0.05f, 0.05f });
    ps->set_float3("lightColor", { .3f, .3f, .3f });
    ps->set_float3("lightDirection", { 1.f, -0.4f, 1.f });
    ps->set_float3("cameraPos", app::instance()->camera()->position());

    ps->copy_all_buffers();
    ps->bind();

    ID3D11Buffer* nothing{};
    constexpr u32 stride = sizeof(vertex_position_normal_texture);
    u32           offset{};
    core::get_device_context()->IASetVertexBuffers(0, 1, &nothing, &stride, &offset);
    core::get_device_context()->IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);
    core::get_device_context()->Draw(3, 0);
}

void render_pointlight(const math::vec3& pos, const math::matrix& world, const math::vec3& light_color, f32 radius, f32 intensity,
                       f32 falloff)
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
    ps->set_float("radius", radius);
    ps->set_float("intensity", intensity);
    ps->set_float("falloff", falloff);

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

void add_pointlight(game_object* obj, pointlight_component* light)
{
    pointlights.push_back({ obj, light });
}

void render_all_pointlights()
{
    for (auto pl : pointlights)
    {
        auto* obj   = pl.first;
        auto* light = pl.second;

        render_pointlight(obj->position(), obj->world_transformation(), light->light_color(), light->radius(), light->intensity(),
                          light->falloff());
    }
}

ID3D11SamplerState* default_sampler_state()
{
    return sampler_state;
}

} // namespace yae::gfx