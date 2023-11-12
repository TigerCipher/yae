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

namespace yae
{
class application
{
public:
    application()  = default;
    ~application() = default;

    bool init(i32 width, i32 height, HWND hwnd) { return true; }
    void shutdown(){}
    bool frame() { return true; }

private:
    bool render() { return true; }
};
} // namespace yae