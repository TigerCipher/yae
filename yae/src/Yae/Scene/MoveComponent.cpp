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

namespace yae
{
void move_component::update(f32 delta)
{
    if (input::key_down('W'))
    {
        m_owner->set_position(m_owner->transformation().position_vector() + m_owner->transformation().forward() * m_speed * delta);
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
}