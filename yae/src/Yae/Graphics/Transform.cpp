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

namespace yae
{

namespace
{
constexpr math::vector x_axis = { 1.f, 0.f, 0.f, 1.f };
constexpr math::vector y_axis = { 0.f, 1.f, 0.f, 1.f };
constexpr math::vector z_axis = { 0.f, 0.f, 1.f, 1.f };
} // anonymous namespace

transform::transform()
{
    m_rot_quat = XMQuaternionIdentity();
}

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

void transform::rotate(f32 angle, axis axis)
{
    switch (axis)
    {
    case axis::x:
        //m_rot_quat = XMQuaternionMultiply(m_rot_quat, XMQuaternionRotationAxis(x_axis, angle * math::deg2rad_multiplier));
        m_rot_quat = XMQuaternionMultiply(XMQuaternionRotationAxis(x_axis, angle * math::deg2rad_multiplier), m_rot_quat);
        //m_rot_quat = XMQuaternionRotationAxis(x_axis, angle * math::deg2rad_multiplier);
        break;
    case axis::y:
        //m_rot_quat = XMQuaternionMultiply(m_rot_quat, XMQuaternionRotationAxis(y_axis, angle * math::deg2rad_multiplier));
        m_rot_quat = XMQuaternionMultiply(XMQuaternionRotationAxis(y_axis, angle * math::deg2rad_multiplier), m_rot_quat);
        //m_rot_quat = XMQuaternionRotationAxis(y_axis, angle * math::deg2rad_multiplier);
        break;
    case axis::z:
        //m_rot_quat = XMQuaternionMultiply(m_rot_quat, XMQuaternionRotationAxis(z_axis, angle * math::deg2rad_multiplier));
        m_rot_quat = XMQuaternionMultiply(XMQuaternionRotationAxis(z_axis, angle * math::deg2rad_multiplier), m_rot_quat);
        //m_rot_quat = XMQuaternionRotationAxis(z_axis, angle * math::deg2rad_multiplier);
        break;
    }
    m_recalculate = true;
}

void transform::rotate(f32 angle, const math::vector& axis)
{
    //m_rot_quat    = XMQuaternionMultiply(m_rot_quat, XMQuaternionRotationAxis(axis, angle * math::deg2rad_multiplier));
    m_rot_quat    = XMQuaternionMultiply(XMQuaternionRotationAxis(axis, angle * math::deg2rad_multiplier), m_rot_quat);
    m_recalculate = true;
}

void transform::rotate(const math::vector& quat)
{
    m_rot_quat    = XMQuaternionMultiply(quat, m_rot_quat);
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
    m_rot_mat     = XMMatrixRotationQuaternion(m_rot_quat);
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