// ------------------------------------------------------------------------------
//
// yae
//    Copyright 2023 Matthew Rogers
//
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.
//
// File Name: D3D11Core
// Date File Created: 11/11/2023
// Author: Matt
//
// ------------------------------------------------------------------------------

#pragma once

#include "D3D11Common.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

namespace yae::gfx::core
{

template<typename T>
constexpr void release(T*& resource)
{
    if (resource)
    {
        resource->Release();
        resource = nullptr;
    }
}

template<typename T>
constexpr void shutdown(T*& resource)
{
    if (resource)
    {
        resource->shutdown();
        delete resource;
        resource = nullptr;
    }
}

bool init(i32 width, i32 height, HWND hwnd, bool fullscreen, f32 screen_depth, f32 screen_near);
void shutdown();

void begin_scene(f32 r, f32 g, f32 b, f32 a);
void end_scene();

ID3D11Device*        get_device();
ID3D11DeviceContext* get_device_context();

math::matrix get_projection_matrix();
math::matrix get_world_matrix();
math::matrix get_orthographic_matrix();

void set_world_matrix(const math::matrix& mat);

void get_video_card_info(char* card_name, i32& memory);

void set_back_buffer_render_target();
void reset_viewport();

void enable_zbuffer();
void disable_zbuffer();

} // namespace yae::gfx::core