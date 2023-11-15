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
//  File Name: Popup.cpp
//  Date File Created: 11/14/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#include "Popup.h"

#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif
#include "Yae/Types.h"
#include "Logger.h"

#include <Windows.h>
#include <algorithm>

#include <string>

namespace yae::popup
{

namespace
{
#ifdef UNICODE
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
#endif


u32 get_icon(style style)
{
    switch (style)
    {
    case style::info: return MB_ICONINFORMATION;
    case style::warning: return MB_ICONWARNING;
    case style::error: return MB_ICONERROR;
    case style::question: return MB_ICONQUESTION;
    }

    return MB_ICONEXCLAMATION;
}

u32 get_button(buttons btns)
{
    switch (btns)
    {
    case buttons::ok_cancel: return MB_OKCANCEL;
    case buttons::yes_no: return MB_YESNO;
    case buttons::quit:
    case buttons::ok: return MB_OK;
    }

    return MB_OK;
}

selection get_selection(i32 response, buttons btns)
{
    switch (response)
    {
    case IDOK: return btns == buttons::quit ? selection::quit : selection::ok;
    case IDCANCEL: return selection::cancel;
    case IDYES: return selection::yes;
    case IDNO: return selection::no;
    default: return selection::none;
    }
}

} // anonymous namespace

selection show(const char* msg, const char* title, style style, buttons btns)
{
    const u32 flags = MB_TASKMODAL | get_icon(style) | get_button(btns);

#ifdef UNICODE
    std::wstring wide_msg;
    std::wstring wide_title;
    if (!utf8_to_utf16(msg, wide_msg) || !utf8_to_utf16(title, wide_title))
    {
        return selection::error;
    }

    const wchar_t* msg_arg   = wide_msg.c_str();
    const wchar_t* title_arg = wide_title.c_str();
#else
    const char* msg_arg   = msg;
    const char* title_arg = title;
#endif

#ifdef _DEBUG
    switch (style)
    {
    case style::warning: LOG_WARN("Showing popup, msg: {}", msg); break;
    case style::error: LOG_ERROR("Showing popup, msg: {}", msg); break;
    case style::info:
    case style::question: LOG_INFO("Showing popup, msg: {}", msg); break;
    }
#endif

    return get_selection(MessageBox(nullptr, msg_arg, title_arg, flags), btns);
}

selection show(const wchar_t* msg, const wchar_t* title, style style, buttons btns)
{
    const u32 flags = MB_TASKMODAL | get_icon(style) | get_button(btns);

#ifdef _DEBUG
    std::wstring msg_str = msg;
    std::string str;
    std::ranges::transform(msg_str, std::back_inserter(str), [](wchar_t c){return (char)c;});
    switch (style)
    {
    case style::warning: LOG_WARN("Showing popup, msg: {}", str); break;
    case style::error: LOG_ERROR("Showing popup, msg: {}", str); break;
    case style::info:
    case style::question: LOG_INFO("Showing popup, msg: {}", str); break;
    }
#endif

    return get_selection(MessageBoxW(nullptr, msg, title, flags), btns);
}

} // namespace yae::popup