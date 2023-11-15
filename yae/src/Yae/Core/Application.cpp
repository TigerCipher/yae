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
//  File Name: Application.cpp
//  Date File Created: 11/12/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------
#include "Application.h"


#include "Yae/Graphics/D3D11Core.h"

namespace yae
{
bool application::init(i32 width, i32 height, HWND hwnd)
{
    if (!gfx::core::init(width, height, hwnd, g_settings->get<bool>("display", "fullscreen"), 1000.f, 0.3f))
    {
        MessageBox(hwnd, L"Failed to initialize DirectX", L"Error", MB_OK);
        return false;
    }

    m_camera = new gfx::camera{};
    m_camera->set_position(0.f, 0.f, -5.f);

    {
        gfx::shader_layout layout{};
        layout.add<math::vec3>("POSITION");
        layout.add<math::vec4>("COLOR");
        m_test_color_shader = gfx::create_shader<gfx::shader>("Color", layout);
        if (!m_test_color_shader)
        {
            MessageBox(hwnd, L"Failed to initialize the test shader", L"Error", MB_OK);
            return false;
        }
    }

    {
        gfx::shader_layout layout{};
        layout.add<math::vec3>("POSITION");
        layout.add<math::vec2>("TEXCOORD");
        m_test_texture_shader = gfx::create_shader<gfx::shader>("Texture", layout);
        if (!m_test_texture_shader)
        {
            MessageBox(hwnd, L"Failed to initialize the test texture shader", L"Error", MB_OK);
            return false;
        }
    }

    m_model = new gfx::model{};
    std::vector<gfx::vertex_position_texture> verts{ 3 };
    std::vector<u32>                          indices(3);

    // bottom left
    verts[0].position = { -1.f, -1.f, 0.f };
    verts[0].texture  = { 0.f, 1.f };

    // top middle
    verts[1].position = { 0.f, 1.f, 0.f };
    verts[1].texture  = { 0.5f, 0.f };

    // bottom right
    verts[2].position = { 1.f, -1.f, 0.f };
    verts[2].texture  = { 1.f, 1.f };

    indices[0] = 0; // Bottom left
    indices[1] = 1; // Top middle
    indices[2] = 2; // Bottom right
    if (!m_model->init(verts, indices))
    {
        MessageBox(hwnd, L"Failed to initialize the test model", L"Error", MB_OK);
        return false;
    }

    m_bricks_texture = new gfx::texture{};
    if (!m_bricks_texture->init("./assets/textures/bricks.tga"))
    {
        MessageBox(hwnd, L"Failed to initialize the texture", L"Error", MB_OK);
        return false;
    }

    return true;
}

void application::shutdown()
{
    gfx::core::shutdown(m_bricks_texture);
    gfx::core::shutdown(m_test_texture_shader);
    gfx::core::shutdown(m_test_color_shader);
    gfx::core::shutdown(m_model);

    SAFE_DELETE(m_camera);

    gfx::core::shutdown();
}

bool application::frame() const
{
    const bool result = render();
    return result;
}

bool application::render() const
{
    gfx::core::begin_scene(0.f, 0.f, 0.f, 1.f);

    m_camera->render();
    m_model->render();


    m_test_texture_shader->set_parameters({ [&] { gfx::shader::set_texture(m_bricks_texture->texture_view()); } });
    if (!m_test_texture_shader->render(m_model->index_count(), m_camera->view()))
    {
        return false;
    }


    gfx::core::end_scene();
    return true;
}
} // namespace yae