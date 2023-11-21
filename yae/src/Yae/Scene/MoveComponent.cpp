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
    if (input::key_down(m_forward))
    {
        m_owner->set_position(m_owner->transformation().position_vector() +
                              m_owner->transformation().forward() * m_speed * delta);
    }

    if (input::key_down(m_left))
    {
        m_owner->set_position(m_owner->transformation().position_vector() + m_owner->transformation().left() * m_speed * delta);
    }

    if (input::key_down(m_right))
    {
        m_owner->set_position(m_owner->transformation().position_vector() + m_owner->transformation().right() * m_speed * delta);
    }

    if (input::key_down(m_back))
    {
        m_owner->set_position(m_owner->transformation().position_vector() + m_owner->transformation().back() * m_speed * delta);
    }

    if (input::key_down(m_up))
    {
        m_owner->set_position(m_owner->transformation().position_vector() + m_owner->transformation().up() * m_speed * delta);
    }

    if (input::key_down(m_down))
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
        f32 angle = delta_pos.x * m_sensitivity * delta;
        //angle     = math::wrap_angle_deg(angle); // TODO get current yaw and add to angle to wrap
        m_owner->rotate(angle, axis::y, true);
    }
    if (rotx)
    {
        f32 angle = delta_pos.y * m_sensitivity * delta;
        //angle     = std::clamp(angle, -89.f, 89.f); // TODO get current pitch and add to angle to clamp
        m_owner->rotate(angle, axis::x);
    }

    if (rotx || roty)
    {
        input::center_cursor();
    }
}

} // namespace yae