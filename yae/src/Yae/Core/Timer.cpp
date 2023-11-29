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
//  File Name: Timer.cpp
//  Date File Created: 11/16/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------
#include "Timer.h"

#include "Win32Header.h"

namespace yae
{
bool timer::start()
{
    i64 freq;
    QueryPerformanceFrequency((LARGE_INTEGER*) &freq);
    if (!freq)
    {
        return false;
    }

    m_frequency = (f32) freq;
    QueryPerformanceCounter((LARGE_INTEGER*) &m_start);

    return true;
}

void timer::frame()
{
    i64 cur;
    QueryPerformanceCounter((LARGE_INTEGER*) &cur);

    i64 elapsed = cur - m_start;

    m_frame_time = (f32)elapsed / m_frequency;
    m_start = cur;
}
} // namespace yae