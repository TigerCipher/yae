﻿//  ------------------------------------------------------------------------------
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
{
    math::vec3 position{};
};

struct vertex_position_color : vertex
{
    math::vec4 color{};
};

struct vertex_position_texture : vertex
{
    math::vec2 texture{};
};

struct vertex_position_texture_normal : vertex_position_texture
{
    math::vec3 normal{};
};

} // namespace yae::gfx