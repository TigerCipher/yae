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
//  File Name: Popup.h
//  Date File Created: 11/14/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#pragma once

namespace yae::popup
{

enum class style
{
    info,
    warning,
    error,
    question,
};

enum class buttons
{
    ok,
    ok_cancel,
    yes_no,
    quit,
};

enum class selection
{
    ok,
    cancel,
    yes,
    no,
    quit,
    none,
    error,
};

constexpr style default_style = style::info;
constexpr buttons default_buttons = buttons::ok;

selection show(const char* msg, const char* title, style style, buttons btns);

inline selection show(const char* msg, const char* title, style style)
{
    return show(msg, title, style, default_buttons);
}

inline selection show(const char* msg, const char* title, buttons btns)
{
    return show(msg, title, default_style, btns);
}

inline selection show(const char* msg, const char* title)
{
    return show(msg, title, default_style, default_buttons);
}


selection show(const wchar_t* msg, const wchar_t* title, style style, buttons btns);

inline selection show(const wchar_t* msg, const wchar_t* title, style style)
{
    return show(msg, title, style, default_buttons);
}

inline selection show(const wchar_t* msg, const wchar_t* title, buttons btns)
{
    return show(msg, title, default_style, btns);
}

inline selection show(const wchar_t* msg, const wchar_t* title)
{
    return show(msg, title, default_style, default_buttons);
}

} // namespace yae::popup