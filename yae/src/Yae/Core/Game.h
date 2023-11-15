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
//  File Name: Game.h
//  Date File Created: 11/14/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#pragma once

#include "Yae/Common.h"
#include "Yae/Graphics/Camera.h"

namespace yae
{
class game
{
public:
    game()          = default;
    virtual ~game() = default;

    virtual bool init()     = 0;
    virtual bool render()   = 0;
    virtual void shutdown() = 0;

    void set_camera(gfx::camera* cam) { m_camera = cam; }

protected:
    gfx::camera* m_camera{};
};
} // namespace yae