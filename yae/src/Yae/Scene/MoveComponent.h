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
//  File Name: MoveComponent.h
//  Date File Created: 11/17/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#pragma once
#include "GameComponent.h"

namespace yae
{

// Free move
class move_component : public game_component
{
public:
    move_component(f32 speed = 10.f) : m_speed{speed} {}
    ~move_component() override = default;

    void update(f32 delta) override;

private:
    f32 m_speed{};
};

}