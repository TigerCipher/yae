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
//  Date File Created: 11/18/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------
#include "Camera.h"

namespace yae::gfx
{

namespace
{
math::matrix default_view{};
} // anonymous namespace

void camera::rotate(f32 dx, f32 dy, f32 delta)
{
    m_yaw         = math::wrap_angle_rad(m_yaw + dx * m_sensitivity * delta * math::deg2rad_multiplier);
    m_pitch       = std::clamp(m_pitch + dy * m_sensitivity * delta * math::deg2rad_multiplier, 0.995f * -math::pi / 2.f,
                               0.995f * math::pi / 2.f);
    m_recalculate = true;
}

void camera::move(f32 x, f32 y, f32 z)
{
    math::vec3 translation = { x, y, z };
    XMStoreFloat3(&translation, XMVector3Transform(XMLoadFloat3(&translation), XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, 0.f) *
                                                                                   XMMatrixScaling(m_speed, m_speed, m_speed)));

    m_position    = { m_position.x + translation.x, m_position.y + translation.y, m_position.z + translation.z };
    m_recalculate = true;
}

math::matrix camera::view()
{
    if (!m_recalculate)
    {
        return m_view;
    }

    m_recalculate = false;

    constexpr math::vector forward{ 0.f, 0.f, 1.f, 0.f };
    const auto             look   = XMVector3Transform(forward, XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, 0.f));
    const auto             pos    = XMLoadFloat3(&m_position);
    const auto             target = pos + look;
    m_view                        = XMMatrixLookAtLH(pos, target, { 0.f, 1.f, 0.f, 0.f });
    return m_view;
}

void create_default_view_matrix()
{
    camera cam{};
    default_view = cam.view();
}

const math::matrix& default_view_matrix()
{
    return default_view;
}

} // namespace yae::gfx