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
#include "Yae/Graphics/Shaders/Shader.h"
#include "GameObject.h"
#include "Yae/Graphics/Model.h"
#include "Yae/Graphics/Texture.h"
#include "Yae/Graphics/Bitmap.h"

namespace yae
{

class game_component
{
public:
    game_component()          = default;
    virtual ~game_component() = default;

    void set_owner(game_object* owner) { m_owner = owner; }

    virtual void update(f32 delta) {}
    virtual bool render(gfx::shader* shader) { return true; }

protected:
    game_object* m_owner{};
};

class model_component : public game_component
{
public:
    model_component(const std::string_view filename);
    model_component(gfx::model* model) : m_model{model} {} 
    ~model_component() override;
    bool render(gfx::shader* shader) override;

private:
    gfx::model* m_model{};
};

class texture_component : public game_component
{
public:
    texture_component(const char* filename);
    ~texture_component() override = default;

    bool render(gfx::shader* shader) override;

private:
    gfx::texture m_texture{};
};

} // namespace yae