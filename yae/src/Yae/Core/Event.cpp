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
//  File Name: Event.cpp
//  Date File Created: 11/17/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------
#include "Event.h"

namespace yae::events
{

namespace
{
constexpr const char* event_names[] = { "app_quit",        "key_pressed", "key_released", "button_pressed",
                                        "button_released", "mouse_moved", "mouse_wheel",  "resized" };

static_assert(_countof(event_names) == code::count - 1);

constexpr u32 max_message_codes = 2 << 13;

struct registered_event
{
    void*         listener;
    func_on_event callback;
};

struct event_code_entry
{
    std::vector<registered_event> events{};
};

event_code_entry registered[max_message_codes];

} // anonymous namespace


void shutdown()
{
    for (auto& [events] : registered)
    {
        if (!events.empty())
        {
            events.clear();
        }
    }

    LOG_INFO("Events submodule shutdown");
}

bool register_listener(u16 code, void* listener, func_on_event on_event)
{
    const u64 register_count = registered[code].events.size();
    for (u64 i = 0; i < register_count; ++i)
    {
        if (registered[code].events[i].listener == listener)
        {
            LOG_WARN("Attempted to register a duplicate {} event listener", event_names[code - 1]);
            return false;
        }
    }

    const registered_event evt{ listener, on_event };
    registered[code].events.push_back(evt);
    LOG_TRACE("Registered listener for event {}", event_names[code - 1]);
    return true;
}

bool unregister_listener(u16 code, void* listener, func_on_event on_event)
{
    std::vector<registered_event>& events = registered[code].events;
    for (u64 i = 0; i < events.size(); ++i)
    {
        if (auto [lsn, callback]{ events[i] }; lsn == listener && callback == on_event)
        {
            LOG_TRACE("Unregistered listener for event {}", event_names[code - 1]);
            events.erase(events.begin() + i);
            return true;
        }
    }

    return false;
}

bool fire(u16 code, void* sender, void* userdata)
{
    LOG_DEBUG("Firing event: {}", event_names[code - 1]);
    std::vector<registered_event>& events = registered[code].events;
    for (u64 i = 0; i < events.size(); ++i)
    {
        if (auto [listener, callback]{ events[i] }; callback(code, sender, listener, userdata))
        {
            // If a listener "handled" an event, don't let remaining listeners handle as well
            return true;
        }
    }

    return false;
}

} // namespace yae::events