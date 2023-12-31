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
//  File Name: Material.h
//  Date File Created: 11/25/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#pragma once

#include "Yae/Common.h"
#include "Texture.h"

namespace yae::gfx
{

struct material
{
    ID3D11ShaderResourceView* diffuse{};
    ID3D11ShaderResourceView* normal{};
    ID3D11ShaderResourceView* blend{};
    math::vec4                tint{ 1.f, 1.f, 1.f, 1.f };
    f32                       specular_power{ 32.f }; // TODO: Currently useless
    ID3D11SamplerState*       sampler{};
};

} // namespace yae::gfx