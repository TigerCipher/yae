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
//  File Name: Camera.cpp
//  Date File Created: 11/12/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------
#include "Camera.h"

using namespace DirectX;

namespace yae::gfx
{
void camera::render()
{
    constexpr math::vec3 up{ 0.f, 1.f, 0.f };
    const math::vec3     pos = m_position;
    constexpr math::vec3 look_at{ 0.f, 0.f, 1.f };

    math::vector look_at_v = XMLoadFloat3(&look_at);

    const f32 pitch = m_rotation.x * math::deg2rad_multiplier;
    const f32 yaw   = m_rotation.y * math::deg2rad_multiplier;
    const f32 roll  = m_rotation.z * math::deg2rad_multiplier;

    const math::matrix rotation = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

    look_at_v = XMVector3TransformCoord(look_at_v, rotation);

    math::vector up_v = XMLoadFloat3(&up);
    up_v              = XMVector3TransformCoord(up_v, rotation);

    const math::vector pos_v = XMLoadFloat3(&pos);
    look_at_v                = XMVectorAdd(pos_v, look_at_v);

    m_view = XMMatrixLookAtLH(pos_v, look_at_v, up_v);
}
} // namespace yae::gfx