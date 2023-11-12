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

    m_model = new gfx::model{};

    if(!m_model->init())
    {
        MessageBox(hwnd, L"Failed to initialize the model", L"Error", MB_OK);
        return false;
    }

    m_color_shader = new gfx::color_shader{};
    if(!m_color_shader->init())
    {
        MessageBox(hwnd, L"Failed to initialize the shader", L"Error", MB_OK);
        return false;
    }

    return true;
}

void application::shutdown()
{

    gfx::core::shutdown(m_color_shader);
    gfx::core::shutdown(m_model);

    SAFE_DELETE(m_camera);

    gfx::core::shutdown();
}

bool application::frame()
{
    bool result = render();
    return result;
}

bool application::render()
{
    gfx::core::begin_scene(0.f, 0.f, 0.f, 1.f);

    m_camera->render();
    m_model->render();

    if(!m_color_shader->render(m_model->index_count(), m_camera->view()))
    {
        return false;
    }

    gfx::core::end_scene();
    return true;
}
} // namespace yae