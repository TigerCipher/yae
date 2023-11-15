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
//  File Name: Entrypoint.h
//  Date File Created: 11/14/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#pragma once

#pragma message("This file should only be included once, in your 'Main.cpp'")
#include "Yae.h"


extern yae::game* create_game();
extern void pre_init();
extern const char* game_name;
extern const char* game_version;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    yae::init(game_name, game_version);
    
    pre_init();

    bool result = yae::system::init(create_game());

    if (result)
    {
        yae::system::run();
    }

    yae::system::shutdown();
    yae::shutdown();
    return 0;
}