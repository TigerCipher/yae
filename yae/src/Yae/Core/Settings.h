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
//  File Name: Settings.h
//  Date File Created: 11/11/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#pragma once

#include "Yae/Types.h"


#include <sstream>
#include <charconv>
#include <string>
#include <unordered_map>

#include "mini/ini.h"

namespace yae
{
class settings
{
public:
    settings(const std::string& game_name, const std::string& version);
    ~settings();

    void load();
    void save(bool regenerate = false);

    //void set(const std::string& key, const std::string& value);
    //void set(const std::string& key, bool value);

    template<typename T>
    void set(const std::string& category, const std::string& key, const T& value)
    {
        if constexpr (std::is_same_v<T, bool>)
        {
            m_ini[category][key] = value ? "true" : "false";
        } else if constexpr (std::is_same_v<T, std::string>)
        {
            m_ini[category][key] = value;
        } else
        {
            m_ini[category][key] = std::to_string(value);
        }
    }

    template<typename T>
    T get(const std::string& category, const std::string& key)
    {
        if (m_ini.has(category) && m_ini[category].has(key))
        {
            return convert_to_type<T>(m_ini[category][key]);
        }
        // log warning/error
        return T{};
    }

private:

    void create_default_settings();

    mINI::INIFile* m_file;
    mINI::INIStructure m_ini;

    template<typename T>
    T convert_to_type(const std::string& s)
    {
        T result;
        if constexpr (std::is_same_v<T, bool>)
        {
            std::istringstream iss{ s };
            iss >> std::boolalpha >> result;
        } else if constexpr (std::is_same_v<T, std::string>)
        {
            result = s;
        } else
        {
            auto res = std::from_chars(s.data(), s.data() + s.size(), result);
            if (res.ec != std::errc{})
            {
                throw std::invalid_argument("Failed to convert from string");
            }
        }

        return result;
    }
};
} // namespace yae