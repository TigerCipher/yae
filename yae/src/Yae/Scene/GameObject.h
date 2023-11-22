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
#include "Yae/Graphics/Transform.h"

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
    game_object* add(game_object& child);
    void         remove(game_object* child);
    //void         remove_unmanged(game_object* child);

    bool render();
    void update(f32 delta);

    void set_position(const math::vec3& pos) { m_transform.set_position(pos); }
    void set_position(const math::vector& pos) { m_transform.set_position(pos); }
    void set_position(f32 x, f32 y, f32 z) { m_transform.set_position(x, y, z); }

    void set_scale(const math::vec3& scale) { m_transform.set_scale(scale); }
    void set_scale(f32 x, f32 y, f32 z) { m_transform.set_scale(x, y, z); }
    void set_scale(f32 scale) { m_transform.set_scale(scale); }

    void rotate(f32 angle, axis axis, bool worldspace = false) { m_transform.rotate(angle, axis, worldspace); }
    void rotate(f32 angle, const math::vector& axis, bool worldspace = false) { m_transform.rotate(angle, axis, worldspace); }
    void set_rotation(f32 x, f32 y, f32 z) { m_transform.set_rotation(x, y, z); }


    constexpr const math::matrix& world_transformation() const { return m_transform.transformation(); }
    constexpr const math::vec3&   position() const { return m_transform.position(); }
    constexpr const math::vec3&   scale() const { return m_transform.scale(); }

    constexpr transform& transformation() { return m_transform; }

    //constexpr const math::vector& forward() const { return m_forward; }

protected:
    void set_parent(game_object* parent) { m_parent = parent; }


    std::vector<game_object*>    m_children{};
    std::vector<game_object*>    m_children_unmanaged{};
    std::vector<game_component*> m_components{};
    game_object*                 m_parent{};

    transform m_transform{};
};

} // namespace yae