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

#include "Yae/Types.h"
#include <DirectXMath.h>

// Because it annoys me to keep including this when I want to use operator overloads. Sue me
using namespace DirectX;

namespace yae::math
{
constexpr f32 epsilon = 1e-5f;

constexpr f32 pi = DirectX::XM_PI;

constexpr f32 deg2rad_multiplier = pi / 180.0f;
constexpr f32 rad2deg_multiplier = 180.0f / pi;

using vec2   = DirectX::XMFLOAT2;
using vec3   = DirectX::XMFLOAT3;
using vec4   = DirectX::XMFLOAT4;
using vector = DirectX::XMVECTOR;
using mat4   = DirectX::XMFLOAT4X4;
using matrix = DirectX::XMMATRIX;


template<primitive_type T>
constexpr T abs(T x)
{
    return x < 0 ? -x : x;
}

constexpr bool near_equal(f32 x, f32 y, f32 tolerance = epsilon)
{
    return abs(x - y) <= tolerance;
}

constexpr bool near_zero(f32 x, f32 tolerance = epsilon)
{
    return abs(x) <= tolerance;
}

inline bool are_matrices_equal(const matrix& matrix1, const matrix& matrix2)
{
    for (int row = 0; row < 4; ++row)
    {
        for (int col = 0; col < 4; ++col)
        {
            if (!near_equal(matrix1.r[row].m128_f32[col], matrix2.r[row].m128_f32[col]))
            {
                return false;
            }
        }
    }
    return true;
}

constexpr bool are_vectors_equal(const vec3& vec1, const vec3& vec2)
{
    return near_equal(vec1.x, vec2.x) && near_equal(vec1.y, vec2.y) && near_equal(vec1.z, vec2.z);
}

} // namespace yae::math

namespace yae
{

enum class axis
{
    x,
    y,
    z
};

}
