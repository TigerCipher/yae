// ------------------------------------------------------------------------------
//
// yae
//    Copyright 2023 Matthew Rogers
//
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.
//
// File Name: System
// Date File Created: 11/11/2023
// Author: Matt
//
// ------------------------------------------------------------------------------
#pragma once

#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
    #define NOMINMAX
#endif
#include <Windows.h>

#include <set>

#include "Yae/Common.h"
#include "Game.h"

namespace yae::system
{

struct resolution
{
    i32 width;
    i32 height;

    bool operator<(const resolution& other) const
    {
        return (width < other.width) || (width == other.width && height < other.height);
    }
};

bool init(game* game);
void shutdown();
void run();

LRESULT CALLBACK message_handler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);


LRESULT CALLBACK window_proc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);


const std::set<resolution>& get_resolutions();

HWND handle();

i32 width();
i32 height();

i32 monitor_width();
i32 monitor_height();

} // namespace yae::system
