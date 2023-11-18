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


#include "Event.h"


namespace yae::input
{

namespace
{

constexpr u16 max_keys = 256;

struct keyboard
{
    bool keys[max_keys]{};
};

struct mouse
{
    i32  x{};
    i32  y{};
    bool buttons[button::max_buttons]{};
};

keyboard current_keyboard{};
keyboard old_keyboard{};

mouse current_mouse{};
mouse old_mouse{};

} // anonymous namespace

void update(f32 delta)
{
    old_keyboard = current_keyboard;
    old_mouse    = current_mouse;
}

void process_key(u16 key, bool pressed)
{
    // key state not changed, no need to process
    if (current_keyboard.keys[key] == pressed)
    {
        return;
    }

    current_keyboard.keys[key] = pressed;
    events::fire(pressed ? events::key_pressed : events::key_released, nullptr, &key);
}

void process_button(u16 btn, bool pressed)
{
    if (current_mouse.buttons[btn] == pressed)
    {
        return;
    }

    current_mouse.buttons[btn] = pressed;
    events::fire(pressed ? events::button_pressed : events::button_released, nullptr, &btn);
}

void process_mouse_move(i32 x, i32 y)
{
    if (current_mouse.x == x && current_mouse.y == y)
    {
        return;
    }

    current_mouse.x = x;
    current_mouse.y = y;
    u16 data[2];
    data[0] = x;
    data[1] = y;
    events::fire(events::mouse_moved, nullptr, data);
}

void process_mouse_wheel(i8 delta)
{
    events::fire(events::mouse_wheel, nullptr, &delta);
}

bool key_down(u16 key)
{
    return current_keyboard.keys[key];
}

bool key_released(u16 key)
{
    return old_keyboard.keys[key] && !current_keyboard.keys[key];
}

bool key_up(u16 key)
{
    return !current_keyboard.keys[key];
}

bool key_pressed(u16 key)
{
    return !old_keyboard.keys[key] && current_keyboard.keys[key];
}

bool button_down(u16 btn)
{
    return current_mouse.buttons[btn];
}

bool button_released(u16 btn)
{
    return old_mouse.buttons[btn] && !current_mouse.buttons[btn];
}

bool button_up(u16 btn)
{
    return !current_mouse.buttons[btn];
}

bool button_pressed(u16 btn)
{
    return !old_mouse.buttons[btn] && current_mouse.buttons[btn];
}

void get_mouse_position(i32* x, i32* y)
{
    *x = current_mouse.x;
    *y = current_mouse.y;
}

void get_previous_mouse_position(i32* x, i32* y)
{
    *x = old_mouse.x;
    *y = old_mouse.y;
}

} // namespace yae::input