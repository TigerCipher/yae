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
    bool result = gfx::core::init(width, height, hwnd, g_settings->get<bool>("display", "fullscreen"), 1000.f, 0.1f);
    if (!result)
    {
        MessageBox(hwnd, L"Failed to initialize DirectX", L"Error", MB_OK);
        return false;
    }

    return result;
}

void application::shutdown()
{
    gfx::core::shutdown();
}

bool application::frame()
{
    bool result = render();
    return result;
}

bool application::render()
{
    gfx::core::begin_scene(0.5f, 0.1f, 0.1f, 1.f);

    gfx::core::end_scene();
    return true;
}
} // namespace yae