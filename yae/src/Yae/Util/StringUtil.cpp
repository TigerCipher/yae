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
//  File Name: StringUtil.cpp
//  Date File Created: 11/21/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------
#include "StringUtil.h"


#include "Yae/Types.h"
#include "Logger.h"

#include "Yae/Core/Win32Header.h"

namespace yae::utl
{

std::wstring to_wstring(const std::string& str)
{
    return std::wstring{ str.begin(), str.end() };
}

std::string to_string(const std::wstring& str)
{
    std::string ret{};
    if (!utf16_to_utf8(str.c_str(), ret))
    {
        LOG_DEBUG("Failed to convert wide string to string");
        return {};
    }

    return ret;
}

bool utf8_to_utf16(const char* utf8, std::wstring& utf16)
{
    const i32 count = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, nullptr, 0);
    if (count <= 0)
    {
        return false;
    }

    utf16 = std::wstring((size_t) count, L'\0');
    return MultiByteToWideChar(CP_UTF8, 0, utf8, -1, utf16.data(), count) > 0;
}

bool utf16_to_utf8(const wchar_t* utf16, std::string& utf8)
{
    const i32 count = WideCharToMultiByte(CP_UTF8, 0, utf16, -1, nullptr, 0, nullptr, nullptr);
    if (count <= 0)
    {
        return false;
    }

    utf8 = std::string((size_t) count, '\0');
    return WideCharToMultiByte(CP_UTF8, 0, utf16, -1, utf8.data(), (i32)utf8.size(), nullptr, nullptr) > 0;
}

} // namespace yae::utl