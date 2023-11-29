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
//  File Name: Logger.cpp
//  Date File Created: 11/14/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------
#include "Logger.h"

#include "Yae/Core/Win32Header.h"

namespace yae::logger::detail
{
void output(const level lvl, std::string_view msg)
{
    std::string str;
    switch (lvl)
    {
    case level::trace: str = std::format("[ TRACE ]: {}\n", msg); break;
    case level::debug: str = std::format("[ DEBUG ]: {}\n", msg); break;
    case level::info: str = std::format("[ INFO ]: {}\n", msg); break;
    case level::warn: str = std::format("[ WARNING ]: {}\n", msg); break;
    case level::error: str = std::format("[ ERROR ]: {}\n", msg); break;
    case level::fatal: str = std::format("[ FATAL ]: {}\n", msg); break;
    }

    OutputDebugStringA(str.c_str());
}

} // namespace yae::logger::detail