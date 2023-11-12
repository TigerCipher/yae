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
//  File Name: PathUtil.cpp
//  Date File Created: 11/11/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------
#include "PathUtil.h"
#include <algorithm>

#include <ShlObj.h>

namespace yae::utl::path
{
std::string get_file_name(const std::string& path)
{
    std::string file_name = path;
    std::ranges::replace(file_name, '\\', '/');
    return file_name.substr(file_name.find_last_of('/') + 1);
}
std::string get_file_extension(const std::string& path)
{
    const std::string file_name = get_file_name(path);
    return file_name.substr(file_name.find_last_of('.') + 1);
}
std::string get_file_name_without_extension(const std::string& path)
{
    const std::string file_name = get_file_name(path);
    return file_name.substr(0, file_name.find_last_of('.'));
}
std::string get_directory(const std::string& path)
{
    std::string directory = path;
    std::ranges::replace(directory, '\\', '/');
    return directory.substr(0, directory.find_last_of('/'));
}

std::string get_documents_directory()
{
    wchar_t       path[MAX_PATH];
    const HRESULT hr = SHGetFolderPathW(nullptr, CSIDL_MYDOCUMENTS, nullptr, SHGFP_TYPE_CURRENT, path);
    if (SUCCEEDED(hr))
    {
        char str[1024];
        wcstombs(str, path, 1023);
        //std::ranges::replace(str, '\\', '/');
        return str;
    }

    return {};
}

} // namespace yae::utl::path