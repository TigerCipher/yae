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
//  File Name: CameraComponent.h
//  Date File Created: 11/20/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#pragma once

#include "GameComponent.h"

namespace yae
{
class camera_component : public game_component
{
public:
    camera_component();
    ~camera_component() override = default;

    void update(f32 delta) override;

    void add_to_engine() override;

    constexpr const math::matrix& view() const { return m_view; }
    static math::matrix     default_view;

private:
    math::matrix m_view{};
    bool         m_recalculate{ true };
};
} // namespace yae