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

    //while (!m_children_unmanaged.empty())
    //{
    //    remove(m_children_unmanaged.back());
    //}
}

game_object* game_object::add(game_component* component)
{
    m_components.push_back(component);
    component->set_owner(this);
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

game_object* game_object::add(game_object& child)
{
    m_children_unmanaged.push_back(&child);
    child.set_parent(this);
    return this;
}

void game_object::remove(game_object* child)
{
    if (const auto it = std::ranges::find(m_children, child); it != m_children.end())
    {
        m_children.erase(it);
    }
}
//void game_object::remove_unmanged(game_object* child)
//{
//    if (const auto it = std::ranges::find(m_children_unmanaged, child); it != m_children_unmanaged.end())
//    {
//        m_children_unmanaged.erase(it);
//    }
//}

bool game_object::render(gfx::shader* shader)
{
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

    for (const auto child : m_children_unmanaged)
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
    m_transform.calculate_transformation(m_parent ? &m_parent->transformation() : nullptr);
    for (const auto comp : m_components)
    {
        comp->update(delta);
    }

    for (const auto child : m_children)
    {
        child->update(delta);
    }

    for (const auto child : m_children_unmanaged)
    {
        child->update(delta);
    }

    //m_transform.calculate_transformation(m_parent ? &m_parent->transformation() : nullptr);
}

} // namespace yae