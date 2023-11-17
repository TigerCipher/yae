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

#pragma message("This Entrypoint header can only be included once, in your 'Main.cpp'")
#include "Yae.h"

#ifdef _DEBUG
    #include <crtdbg.h>
#endif


extern yae::game* create_game();
extern void pre_init();
extern const char* game_name;
extern const char* game_version;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#if _DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    // EXAMPLE: If the debugger outputs the following:
    //     Detected memory leaks!
    // Dumping objects ->
    // {380} normal block at 0x000001C3F54F87B0, 40 bytes long.
    //  Data: <         * >   >> 00 00 00 00 00 00 00 00 DC 2A DC 3E DA 04 E9 3E
    // Object dump complete.
    // You see that {380} normal block... ? well put that 380 in the following line and it will breakpoint where that block is allocated
    // so you can identify the memory leak
    //_CrtSetBreakAlloc(380); // break point when block 380 is allocated
#endif
    yae::init(game_name, game_version);
    
    pre_init();

    if (yae::system::init(create_game()))
    {
        yae::system::run();
    }

    yae::system::shutdown();
    yae::shutdown();
    return 0;
}