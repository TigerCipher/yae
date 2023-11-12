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
//  File Name: MathUtil.h
//  Date File Created: 11/11/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#pragma once

#include <DirectXMath.h>


namespace yae::math
{
using vec2   = DirectX::XMFLOAT2;
using vec3   = DirectX::XMFLOAT3;
using vec4   = DirectX::XMFLOAT4;
using vector = DirectX::XMVECTOR;
using mat4   = DirectX::XMFLOAT4X4;
using matrix = DirectX::XMMATRIX;
} // namespace yae::math
