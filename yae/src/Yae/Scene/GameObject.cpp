﻿//  ------------------------------------------------------------------------------
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
//  File Name: GameObject.cpp
//  Date File Created: 11/16/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------
#include "GameObject.h"

#include "GameComponent.h"

using namespace DirectX;

namespace yae
{
game_object::~game_object()
{
    while (!m_components.empty())
    {
        auto& comp = m_components.back();
        remove(comp);
        delete comp;
        comp = nullptr;
    }

    while (!m_children.empty())
    {
        auto& child = m_children.back();
        remove(child);
        delete child;
        child = nullptr;
    }
}

game_object* game_object::add(game_component* component)
{
    m_components.push_back(component);
    return this;
}

void game_object::remove(game_component* component)
{
    if (const auto it = std::ranges::find(m_components, component); it != m_components.end())
    {
        m_components.erase(it);
    }
}
game_object* game_object::add(game_object* child)
{
    m_children.push_back(child);
    child->set_parent(this);
    return this;
}

void game_object::remove(game_object* child)
{
    if (const auto it = std::ranges::find(m_children, child); it != m_children.end())
    {
        m_children.erase(it);
    }
}

bool game_object::render(gfx::shader* shader)
{
    shader->set_world(m_transform);
    for (const auto comp : m_components)
    {
        if (!comp->render(shader))
        {
            return false;
        }
    }

    for (const auto child : m_children)
    {
        if (!child->render(shader))
        {
            return false;
        }
    }

    return true;
}

void game_object::update(f32 delta)
{
    calculate_world_transformation();
    for (const auto comp : m_components)
    {
        comp->update(delta);
    }

    for (const auto child : m_children)
    {
        child->update(delta);
    }
    calculate_world_transformation();
}

void game_object::set_position(const math::vec3& pos)
{
    m_position                   = pos;
    m_recalculate_transformation = true;
}

void game_object::set_position(f32 x, f32 y, f32 z)
{
    m_position                   = { x, y, z };
    m_recalculate_transformation = true;
}

void game_object::set_scale(const math::vec3& scale)
{
    m_scale                      = scale;
    m_recalculate_transformation = true;
}

void game_object::set_scale(f32 x, f32 y, f32 z)
{
    m_scale                      = { x, y, z };
    m_recalculate_transformation = true;
}

void game_object::set_scale(f32 scale)
{
    m_scale                      = { scale, scale, scale };
    m_recalculate_transformation = true;
}

void game_object::set_rotation(const math::vec3& rotation)
{
    m_rotation                   = rotation;
    m_recalculate_transformation = true;
}

void game_object::set_rotation(f32 x, f32 y, f32 z)
{
    m_rotation                   = { x, y, z };
    m_recalculate_transformation = true;
}

void game_object::set_rotation(f32 angle, axis axis)
{
    switch (axis)
    {
    case axis::x: m_rotation = { angle, 0.f, 0.f }; break;
    case axis::y: m_rotation = { 0.f, angle, 0.f }; break;
    case axis::z: m_rotation = { 0.f, 0.f, angle }; break;
    }
    m_recalculate_transformation = true;
}

void game_object::calculate_world_transformation()
{
    if (!m_recalculate_transformation && (m_parent && m_parent->m_recalculate_transformation))
    {
        return;
    }

    m_recalculate_transformation = false;

    const math::matrix scale     = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
    const math::matrix rotate    = XMMatrixRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z);
    const math::matrix translate = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);

    if (!m_parent)
    {
        m_transform = XMMatrixMultiply(XMMatrixMultiply(scale, rotate), translate);

    } else
    {
        m_transform = XMMatrixMultiply(XMMatrixMultiply(scale, rotate), translate);
        m_transform = XMMatrixMultiply(m_parent->transform(), m_transform);
    }
}
} // namespace yae