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
//  File Name: Bitmap.h
//  Date File Created: 11/18/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#pragma once

#include "D3D11Common.h"
#include "Texture.h"
#include "Vertex.h"
#include "Shaders/Shader.h"

namespace yae::gfx
{
class bitmap
{
public:
    bitmap(bool coords_are_center = false) : m_coords_are_center(coords_are_center) {}
    ~bitmap();

    bool init(const char* filename, i32 x, i32 y);
    void shutdown();

    bool render(shader* shader, const math::matrix& world);

    constexpr u32 index_count() const { return m_index_count; }

    ID3D11ShaderResourceView* texture_view() const { return m_texture.texture_view(); }

    void set_location(i32 x, i32 y);

private:
    bool          update();
    ID3D11Buffer* m_vertex_buffer{};
    ID3D11Buffer* m_index_buffer{};
    texture       m_texture{};
    u32           m_vertex_count{};
    u32           m_index_count{};
    u32           m_width{};
    u32           m_height{};
    i32           m_x{};
    i32           m_y{};
    i32           m_old_x{ -1 };
    i32           m_old_y{ -1 };
    bool          m_coords_are_center{};
};
} // namespace yae::gfx