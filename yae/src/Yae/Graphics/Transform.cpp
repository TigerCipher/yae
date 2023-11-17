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
//  File Name: Transform.cpp
//  Date File Created: 11/17/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------
#include "Transform.h"

using namespace DirectX;

namespace yae
{

void transform::set_position(const math::vec3& pos)
{
    m_pos         = pos;
    m_pos_vec     = XMLoadFloat3(&m_pos);
    m_recalculate = true;
}
void transform::set_position(const math::vector& pos)
{
    m_pos_vec = pos;
    XMStoreFloat3(&m_pos, pos);
    m_recalculate = true;
}

void transform::set_position(f32 x, f32 y, f32 z)
{
    m_pos         = { x, y, z };
    m_pos_vec     = XMLoadFloat3(&m_pos);
    m_recalculate = true;
}

void transform::set_scale(const math::vec3& scale)
{
    m_scale       = scale;
    m_recalculate = true;
}

void transform::set_scale(f32 x, f32 y, f32 z)
{
    m_scale       = { x, y, z };
    m_recalculate = true;
}

void transform::set_scale(f32 scale)
{
    set_scale(scale, scale, scale);
}

void transform::set_rotation(const math::vec3& rot)
{
    m_rot         = rot;
    m_recalculate = true;
}

void transform::set_rotation(f32 angle, axis axis)
{
    switch (axis)
    {
    case axis::x: m_rot = { angle * math::deg2rad_multiplier, m_rot.y, m_rot.z }; break;
    case axis::y: m_rot = { m_rot.x, angle * math::deg2rad_multiplier, m_rot.z }; break;
    case axis::z: m_rot = { m_rot.x, m_rot.y, angle * math::deg2rad_multiplier }; break;
    }
    m_recalculate = true;
}

void transform::set_rotation(f32 x, f32 y, f32 z)
{
    m_rot         = { x * math::deg2rad_multiplier, y * math::deg2rad_multiplier, z * math::deg2rad_multiplier };
    m_recalculate = true;
}

void transform::rotate(f32 angle, axis axis)
{
    switch (axis)
    {
    case axis::x: m_rot = { m_rot.x + angle * math::deg2rad_multiplier, m_rot.y, m_rot.z }; break;
    case axis::y: m_rot = { m_rot.x, m_rot.y + angle * math::deg2rad_multiplier, m_rot.z }; break;
    case axis::z: m_rot = { m_rot.x, m_rot.y, m_rot.z + angle * math::deg2rad_multiplier }; break;
    }
    m_recalculate = true;
}

void transform::calculate_transformation(const transform* parent)
{
    if (!m_recalculate)
    {
        return;
    }

    m_recalculate = false;

    m_scale_mat   = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
    m_rot_mat     = XMMatrixRotationRollPitchYaw(m_rot.x, m_rot.y, m_rot.z);
    m_translation = XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z);

    m_right   = m_rot_mat.r[0];
    m_up      = m_rot_mat.r[1];
    m_forward = m_rot_mat.r[2];
    m_left    = -m_right;
    m_back    = -m_forward;
    m_down    = -m_up;

    if (!parent)
    {
        m_transformation = XMMatrixMultiply(XMMatrixMultiply(m_scale_mat, m_rot_mat), m_translation);

    } else
    {
        m_transformation = XMMatrixMultiply(XMMatrixMultiply(m_scale_mat, m_rot_mat), m_translation);
        m_transformation = XMMatrixMultiply(m_transformation, parent->transformation());
    }

    constexpr math::vec3 look_at{ 0.f, 0.f, 1.f };
    math::vector         look_at_v = XMLoadFloat3(&look_at);

    //const f32          pitch    = m_rot.x;// * math::deg2rad_multiplier;
    //const f32          yaw      = m_rot.y * math::deg2rad_multiplier;
    //const f32          roll     = m_rot.z * math::deg2rad_multiplier;
    //const math::matrix rotation = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

    look_at_v         = XMVector3TransformCoord(look_at_v, m_rot_mat);
    math::vector up_v = m_up;
    up_v              = XMVector3TransformCoord(up_v, m_rot_mat);

    look_at_v = XMVectorAdd(position_vector(), look_at_v);

    m_view = XMMatrixLookAtLH(position_vector(), look_at_v, up_v);
}

} // namespace yae