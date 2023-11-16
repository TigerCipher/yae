// ------------------------------------------------------------------------------
//
// yae
//    Copyright 2023 Matthew Rogers
//
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.
//
// File Name: GameObject
// Date File Created: 11/16/2023
// Author: Matt
//
// ------------------------------------------------------------------------------

#pragma once

#include "Yae/Common.h"
#include "Yae/Graphics/Shaders/Shader.h"

namespace yae
{

class game_component;

class game_object
{
public:
    game_object() = default;
    virtual ~game_object();

    game_object* add(game_component* component);
    void         remove(game_component* component);
    game_object* add(game_object* child);
    void         remove(game_object* child);

    bool render(gfx::shader* shader);
    void update(f32 delta);

    void set_position(const math::vec3& pos);
    void set_position(f32 x, f32 y, f32 z);

    void set_scale(const math::vec3& scale);
    void set_scale(f32 x, f32 y, f32 z);
    void set_scale(f32 scale);

    void set_rotation(const math::vec3& rotation);
    void set_rotation(f32 x, f32 y, f32 z);
    void set_rotation(f32 angle, axis axis);

    constexpr const math::matrix& transform() const { return m_transform; }
    constexpr const math::vec3&   position() const { return m_position; }
    constexpr const math::vec3&   scale() const { return m_scale; }
    constexpr const math::vec3&   rotation() const { return m_rotation; }

private:
    void calculate_world_transformation();
    void set_parent(game_object* parent) { m_parent = parent; }

    struct transformation
    {
        math::matrix translation{};
        math::matrix scale{};
        math::matrix rotation{};
    };

    std::vector<game_object*>    m_children{};
    std::vector<game_component*> m_components{};
    math::matrix                 m_transform{};
    math::vec3                   m_position{};
    math::vec3                   m_scale{ 1.f, 1.f, 1.f };
    math::vec3                   m_rotation{};
    bool                         m_recalculate_transformation{ true };
    transformation               m_transformation{};
    game_object*                 m_parent{};
};

} // namespace yae