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
//  File Name: Application.h
//  Date File Created: 11/11/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#pragma once

#include "Yae/Common.h"

#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
    #define NOMINMAX
#endif
#include <Windows.h>

#include "Yae/Scene/GameObject.h"
#include "Yae/Graphics/Model.h"
#include "Yae/Graphics/Texture.h"
#include "Yae/Graphics/Shaders/Shader.h"
#include "Yae/Graphics/Camera.h"
#include "Game.h"
#include "Timer.h"

namespace yae
{

namespace app
{
void  set(game* game);
game* instance();
} // namespace app

class application
{
public:
    application(game* game) : m_game{ game } {}
    ~application() = default;

    bool init(i32 width, i32 height, HWND hwnd);
    void shutdown();
    bool frame();

private:
    bool render() const;

    game*        m_game{};
    gfx::camera* m_camera{};
    //game_object* m_camera{};
    timer m_timer{};
};
} // namespace yae