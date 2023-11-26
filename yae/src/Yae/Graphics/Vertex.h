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
//  File Name: Vertex.h
//  Date File Created: 11/13/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#pragma once

#include "D3D11Common.h"

namespace yae::gfx
{

struct vertex
{};

struct vertex_position_color : vertex
{
    math::vec3 position{};
    math::vec4 color{};
    vertex_position_color() = default;
    vertex_position_color(const math::vec3& position, const math::vec4& color) : position(position), color(color) {}
};

struct vertex_position_texture : vertex
{
    math::vec3 position{};
    math::vec2 texture{};

    vertex_position_texture() = default;
    vertex_position_texture(const math::vec3& position, const math::vec2& texture) : position(position), texture(texture) {}
};

struct vertex_position_texture_normal : vertex
{
    math::vec3 position{};
    math::vec2 texture{};
    math::vec3 normal{};

    vertex_position_texture_normal() = default;
    vertex_position_texture_normal(const math::vec3& position, const math::vec2& texture, const math::vec3& normal) :
        position(position), texture(texture), normal(normal)
    {}
};

struct vertex_position_normal_texture : vertex
{
    math::vec3 position{};
    math::vec3 normal{};
    math::vec2 texture{};

    vertex_position_normal_texture() = default;
    vertex_position_normal_texture(const math::vec3& position, const math::vec3& normal, const math::vec2& texture) :
        position(position), normal(normal), texture(texture)
    {}
};

struct vertex_pos_norm_tex_tang : vertex
{
    math::vec3 position{};
    math::vec3 normal{};
    math::vec2 texture{};
    math::vec3 tangent{};
    math::vec3 binormal{};

    vertex_pos_norm_tex_tang() = default;
    vertex_pos_norm_tex_tang(const math::vec3& position, const math::vec3& normal, const math::vec2& texture,
                             const math::vec3& tangent, const math::vec3& binormal) :
        position(position), normal(normal), texture(texture), tangent(tangent), binormal(binormal)
    {}
};

} // namespace yae::gfx