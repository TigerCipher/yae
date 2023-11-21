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
//  File Name: Geometry.h
//  Date File Created: 11/20/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#pragma once

#include "Vertex.h"
#include "Model.h"

namespace yae::gfx::geometry
{

void create_fullscreen_quad(std::vector<vertex_position_normal_texture>& vertices, std::vector<u32>& indices);

void create_sphere(std::vector<vertex_position_normal_texture>& vertices, std::vector<u32>& indices, f32 radius, u32 slice_count = 36, u32 stack_count = 36);

void create_box(std::vector<vertex_position_normal_texture>& vertices, std::vector<u32>& indices, f32 width, f32 height, f32 depth);

model* create_sphere(f32 radius, u32 slice_count = 36, u32 stack_count = 36);
model* create_box(f32 width, f32 height, f32 depth);

}