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
//  File Name: Transform.h
//  Date File Created: 11/17/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#pragma once

#include "Yae/Common.h"

namespace yae
{
class transform
{
public:
    transform()  = default;
    ~transform() = default;

    constexpr const math::vector& position_vector() const { return m_pos_vec; }
    constexpr const math::vec3&   position() const { return m_pos; }
    constexpr const math::vec3&   scale() const { return m_scale; }
    constexpr const math::vec3&   rotation() const { return m_rot; }
    constexpr const math::matrix& transformation() const { return m_transformation; }
    constexpr const math::matrix& rotation_matrix() const { return m_rot_mat; }
    constexpr const math::matrix& scale_matrix() const { return m_scale_mat; }
    constexpr const math::vector& forward() const { return m_forward; }
    constexpr const math::vector& back() const { return m_back; }
    constexpr const math::vector& left() const { return m_left; }
    constexpr const math::vector& right() const { return m_right; }
    constexpr const math::vector& up() const { return m_up; }
    constexpr const math::vector& down() const { return m_down; }

    void set_position(const math::vec3& pos);
    void set_position(const math::vector& pos);
    void set_position(f32 x, f32 y, f32 z);
    void set_scale(const math::vec3& scale);
    void set_scale(f32 x, f32 y, f32 z);
    void set_scale(f32 scale);
    void set_rotation(const math::vec3& rot);
    void set_rotation(f32 angle, axis axis);
    void set_rotation(f32 x, f32 y, f32 z);

    void calculate_transformation(const transform* parent);

private:
    math::vector m_pos_vec{};
    math::vec3   m_pos{};
    math::vec3   m_scale{1.f, 1.f, 1.f};
    math::vec3   m_rot{};
    math::matrix m_transformation{};
    math::matrix m_translation{};
    math::matrix m_rot_mat{};
    math::matrix m_scale_mat{};
    math::vector m_forward{};
    math::vector m_back{};
    math::vector m_left{};
    math::vector m_right{};
    math::vector m_up{};
    math::vector m_down{};

    bool m_recalculate{ true };
};
} // namespace yae