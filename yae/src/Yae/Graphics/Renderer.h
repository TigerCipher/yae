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
//  File Name: Renderer.h
//  Date File Created: 11/22/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#pragma once

#include "Model.h"
#include "Texture.h"
#include "Yae/Scene/GameComponent.h"
#include "Yae/Scene/GameObject.h"

namespace yae::gfx
{

void init_renderer();
void shutdown_renderer();
void render3d(const model* model, const math::matrix& world, const material& mat);
void render2d(const model* model, const texture* tex, const math::matrix& world);

void render_base_to_screen();

void render_pointlight(const math::vec3& pos, const math::matrix& world, const math::vec3& light_color, f32 radius, f32 intensity,
                       f32 falloff);

void add_pointlight(game_object* obj, pointlight_component* light);

void render_all_pointlights();

ID3D11SamplerState* default_sampler_state();

} // namespace yae::gfx