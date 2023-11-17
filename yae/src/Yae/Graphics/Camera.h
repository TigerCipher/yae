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
//  Date File Created: 11/12/2023
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

    void render();

    void set_position(f32 x, f32 y, f32 z) { m_position = { x, y, z }; }

    void set_position(const math::vec3& pos) { m_position = pos; }

    // Angles expected to be in degrees
    void set_rotation(f32 x, f32 y, f32 z) { m_rotation = { x, y, z }; }
    // Angles expected to be in degrees
    void set_rotation(const math::vec3& rot) { m_rotation = rot; }

    constexpr math::vec3   position() const { return m_position; }
    constexpr math::vec3   rotation() const { return m_rotation; }
    constexpr math::matrix view() const { return m_view; }

private:
    math::vec3   m_position{};
    math::vec3   m_rotation{};
    math::vec3   m_old_position{};
    math::vec3   m_old_rotation{};
    math::matrix m_view{};
    bool         m_setup_once{};
};

} // namespace yae::gfx