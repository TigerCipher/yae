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
//  File Name: GameComponent.h
//  Date File Created: 11/16/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#pragma once

#include "Yae/Common.h"
#include "GameObject.h"
#include "Yae/Graphics/Model.h"
#include "Yae/Graphics/Texture.h"

namespace yae
{

class game_component
{
public:
    game_component()          = default;
    virtual ~game_component() = default;

    void                   set_owner(game_object* owner) { m_owner = owner; }
    constexpr game_object* owner() const { return m_owner; }

    virtual void update(f32 delta) {}
    virtual bool render() { return true; }

    virtual void add_to_engine() {}

protected:
    game_object* m_owner{};
};

class model_component : public game_component
{
public:
    model_component(const std::string_view model, const char* texture, const char* blendtex = nullptr);
    model_component(gfx::model* model, const char* texture, gfx::texture* blendtex = nullptr) :
        m_model{ model }, m_blendtex(blendtex)
    {
        m_texture.init(texture);
    }
    ~model_component() override;
    bool render() override;

private:
    gfx::model*   m_model{};
    gfx::texture  m_texture{};
    gfx::texture* m_blendtex{};
};


class bitmap_component : public game_component
{
public:
    bitmap_component(u32 width, u32 height, const char* filename);
    ~bitmap_component() override;
    bool render() override;

private:
    gfx::model*  m_model{};
    gfx::texture m_texture{};
};

class pointlight_component : public game_component
{
public:
    pointlight_component(const math::vec3& light_color, f32 radius, f32 intensity, f32 falloff) :
        m_light_color(light_color), m_radius(radius), m_intensity(intensity), m_falloff(falloff)
    {}
    ~pointlight_component() override = default;
    bool render() override;

    void add_to_engine() override;

    math::vec3 light_color() const { return m_light_color; }
    f32        radius() const { return m_radius; }
    f32        intensity() const { return m_intensity; }
    f32        falloff() const { return m_falloff; }

private:
    math::vec3 m_light_color{};
    f32        m_radius{};
    f32        m_intensity{};
    f32        m_falloff{};
};

} // namespace yae