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
//  File Name: Light.h
//  Date File Created: 11/14/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#pragma once

#include "Yae/Util/MathUtil.h"

namespace yae::gfx
{

/**
 * \brief Basic light struct. It handles:\n\n
 * ambient lighting\n
 * directional lighting\n
 * specular lighting
 */
struct base_light
{
    math::vec4 ambient_color{};
    math::vec4 diffuse_color{};
    math::vec3 direction{};
};

}