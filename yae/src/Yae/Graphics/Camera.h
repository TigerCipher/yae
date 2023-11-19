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
//  File Name: Camera.h
//  Date File Created: 11/18/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#pragma once

#include "Yae/Common.h"

namespace yae::gfx
{

class camera
{
public:
    camera()  = default;
    ~camera() = default;

    void rotate(f32 dx, f32 dy, f32 delta);
    void move(f32 x, f32 y, f32 z, f32 delta);

    math::matrix view();

    void set_speed(f32 speed) { m_speed = speed; }
    void modify_speed(f32 factor) { m_speed *= factor; }

    void set_sensitivity(f32 sens) { m_sensitivity = sens; }

    constexpr const math::vec3& position() const { return m_position; }

    void set_position(f32 x, f32 y, f32 z) { m_position = { x, y, z }; }

private:
    f32        m_pitch{};
    f32        m_yaw{};
    math::vec3 m_position{0.f, 0.f, -10.f};

    f32 m_speed{ 10.f };
    f32 m_sensitivity{ 10.f };

    math::matrix m_view{};
    bool         m_recalculate{ true };
};

void create_default_view_matrix();
const math::matrix& default_view_matrix();

} // namespace yae::gfx