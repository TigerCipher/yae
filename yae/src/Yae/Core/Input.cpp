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
//  File Name: Input.cpp
//  Date File Created: 11/11/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------
#include "Input.h"

namespace yae::input
{

namespace
{
bool keys[256]{};
} // anonymous namespace

void process_key(u32 key, bool pressed)
{
    keys[key] = pressed;
}

bool is_key_down(u32 key)
{
    return keys[key];
}
}