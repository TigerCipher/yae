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
//  File Name: Event.h
//  Date File Created: 11/17/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#pragma once

#include "Yae/Common.h"

namespace yae::events
{


enum code : u16
{
    app_quit = 0x01,
    key_pressed,
    key_released,
    button_pressed,
    button_released,
    mouse_moved,
    mouse_wheel,
    resized,

    count,
    max_code = 0xff
};


using func_on_event = bool (*)(u16 code, void* sender, void* listener, void* userdata);

bool init();

void shutdown();

/**
 * Registers an event/listener/callback combination. Duplicates will return false
 * @param code The event code to listen for
 * @param listener A pointer to the listener. Can be null
 * @param on_event The callback function that will be invoked with the event is fired
 * @return true if the event was registered, false otherwise
 */
bool register_listener(u16 code, void* listener, func_on_event on_event);

/**
 * Unregisters an event/listener/callback combination. Duplicates will return false
 * @param code The event code to stop listening for
 * @param listener A pointer to the listener. Can be null
 * @param on_event The callback function to be unregistered
 * @return true if the event was unregistered, false otherwise
 */
bool unregister_listener(u16 code, void* listener, func_on_event on_event);

/**
 * Fires an event to all listeners of the said event. If handled, it should not be passed to any further listeners
 * @param code The event code to fire
 * @param sender A pointer to the sender. Can be null
 * @param userdata Event data
 * @return true if handled, false otherwise
 */
bool fire(u16 code, void* sender, void* userdata);

} // namespace yae::events