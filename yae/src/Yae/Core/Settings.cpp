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
//  File Name: Settings.cpp
//  Date File Created: 11/11/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#include "Settings.h"

#include "Yae/Util/PathUtil.h"


#include <filesystem>


namespace yae
{
settings::settings(const std::string& game_name)
{
    std::filesystem::create_directories(utl::path::get_documents_directory() + "\\yae\\" + game_name);
    const bool already_exists =
        std::filesystem::exists(utl::path::get_documents_directory() + "\\yae\\" + game_name + "\\settings.ini");
    m_file = new mINI::INIFile(utl::path::get_documents_directory() + "\\yae\\" + game_name + "\\settings.ini");

    if (already_exists)
    {
        load();
    }
    // TODO: else, create default settings
}

settings::~settings()
{
    delete m_file;
}

void settings::load()
{
    m_file->read(m_ini);
}

void settings::save()
{
    m_file->write(m_ini);
}

} // namespace yae