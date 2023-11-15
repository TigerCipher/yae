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
//  File Name: Logger.h
//  Date File Created: 11/14/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#pragma once

#include "Yae/Types.h"

#include <format>

namespace yae::logger
{

enum class level : u8
{
    trace,
    debug,
    info,
    warn,
    error,
    fatal
};

namespace detail
{
void output(level lvl, std::string_view msg);
}

} // namespace yae::logger


#ifdef _DEBUG
    #define LOG_TRACE(msg, ...) yae::logger::detail::output(yae::logger::level::trace, std::format(msg, ##__VA_ARGS__))
    #define LOG_DEBUG(msg, ...) yae::logger::detail::output(yae::logger::level::debug, std::format(msg, ##__VA_ARGS__))
    #define LOG_INFO(msg, ...)  yae::logger::detail::output(yae::logger::level::info, std::format(msg, ##__VA_ARGS__))
    #define LOG_WARN(msg, ...)  yae::logger::detail::output(yae::logger::level::warn, std::format(msg, ##__VA_ARGS__))
    #define LOG_ERROR(msg, ...) yae::logger::detail::output(yae::logger::level::error, std::format(msg, ##__VA_ARGS__))
    #define LOG_FATAL(msg, ...) yae::logger::detail::output(yae::logger::level::fatal, std::format(msg, ##__VA_ARGS__))
#else
    #define LOG_TRACE(msg, ...)
    #define LOG_DEBUG(msg, ...)
    #define LOG_INFO(msg, ...)
    #define LOG_WARN(msg, ...)
    #define LOG_ERROR(msg, ...)
    #define LOG_FATAL(msg, ...)
#endif