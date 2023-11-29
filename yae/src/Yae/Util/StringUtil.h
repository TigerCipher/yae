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
//  File Name: StringUtil.h
//  Date File Created: 11/21/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#pragma once

#include <string>

namespace yae::utl
{

std::wstring to_wstring(const std::string& str);
std::string  to_string(const std::wstring& str);

bool utf8_to_utf16(const char* utf8, std::wstring& utf16);
bool utf16_to_utf8(const wchar_t* utf16, std::string& utf8);

} // namespace yae::utl