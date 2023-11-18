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
//  File Name: MoveComponent.cpp
//  Date File Created: 11/17/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------
#include "MoveComponent.h"

#include "Yae/Core/Input.h"
#include "Yae/Core/System.h"

namespace yae
{
void move_component::update(f32 delta)
{
    if (input::key_down('W'))
    {
        m_owner->set_position(m_owner->transformation().position_vector() +
                              m_owner->transformation().forward() * m_speed * delta);
    }

    if (input::key_down('A'))
    {
        m_owner->set_position(m_owner->transformation().position_vector() + m_owner->transformation().left() * m_speed * delta);
    }

    if (input::key_down('D'))
    {
        m_owner->set_position(m_owner->transformation().position_vector() + m_owner->transformation().right() * m_speed * delta);
    }

    if (input::key_down('S'))
    {
        m_owner->set_position(m_owner->transformation().position_vector() + m_owner->transformation().back() * m_speed * delta);
    }

    if (input::key_down('E'))
    {
        //math::vec3 rot = m_owner->rotation();
        //rot.z += math::deg2rad_multiplier * m_speed * delta;
        //m_owner->set_rotation(rot);
        //m_owner->set_rotation(rot.z * math::rad2deg_multiplier + m_speed * delta, axis::z);
        m_owner->transformation().rotate(m_speed * delta, axis::z);
    }

    if (input::key_down('Q'))
    {
        //math::vec3 rot = m_owner->rotation();
        //rot.z -= math::deg2rad_multiplier * m_speed * delta;
        //m_owner->set_rotation(rot);
        //m_owner->set_rotation(rot.z * math::rad2deg_multiplier - m_speed * delta, axis::z);
        m_owner->transformation().rotate(-m_speed * delta, axis::z);
    }

    if (input::key_down(VK_SPACE))
    {
        m_owner->set_position(m_owner->transformation().position_vector() + m_owner->transformation().up() * m_speed * delta);
    }

    if (input::key_down(VK_SHIFT))
    {
        m_owner->set_position(m_owner->transformation().position_vector() + m_owner->transformation().down() * m_speed * delta);
    }
}


void freelook_component::update(f32 delta)
{
    if (input::button_pressed(input::button::left))
    {
        input::lock_cursor(true, true);
    } else if (input::button_pressed(input::button::right))
    {
        input::lock_cursor(false);
    }

    if (!input::is_cursor_locked())
    {
        return;
    }

    i32 x, y;
    input::get_mouse_position(&x, &y);
    math::vec2 pos       = { (f32) x, (f32) y };
    math::vec2 center    = { (f32) system::width() / 2, (f32) system::height() / 2 };
    math::vec2 delta_pos = { pos.x - center.x, pos.y - center.y };

    bool roty = delta_pos.x != 0;
    bool rotx = delta_pos.y != 0;

    if (roty)
    {
        //f32 new_rot = m_owner->rotation().y /** math::rad2deg_multiplier*/ + delta_pos.x * m_sensitivity * delta * math::deg2rad_multiplier;
        //new_rot = math::wrap_angle_rad(new_rot);
        //m_owner->set_rotation(m_owner->rotation().x * math::rad2deg_multiplier, new_rot * math::rad2deg_multiplier, 0.f);
        math::vector axis = m_owner->transformation().up();
        math::vector rot  = XMQuaternionRotationAxis(axis, delta_pos.x * m_sensitivity * delta * math::deg2rad_multiplier);
        math::vector cur  = XMLoadFloat3(&m_owner->rotation());
        rot += cur;
        math::vec3 newrot;
        XMStoreFloat3(&newrot, rot);
        m_owner->set_rotation(newrot);
    }
    if (rotx)
    {
        //f32 new_rot = m_owner->rotation().x * math::rad2deg_multiplier + delta_pos.y * m_sensitivity * delta;
        //new_rot = std::clamp(new_rot, -89.f, 89.f);
        //m_owner->set_rotation(new_rot, m_owner->rotation().y * math::rad2deg_multiplier, 0.f);
        math::vector axis = m_owner->transformation().right();
        math::vector rot = XMQuaternionRotationAxis(axis, delta_pos.y * m_sensitivity * delta * math::deg2rad_multiplier);
        math::vector cur = XMLoadFloat3(&m_owner->rotation());
        rot += cur;
        math::vec3 newrot;
        XMStoreFloat3(&newrot, rot);
        m_owner->set_rotation(newrot);
    }

    if (rotx || roty)
    {
        input::center_cursor();
    }
}

} // namespace yae