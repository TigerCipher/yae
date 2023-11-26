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
//  File Name: AssetManager.cpp
//  Date File Created: 11/25/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------
#include "AssetManager.h"

namespace yae::assets
{
namespace
{
std::unordered_map<std::string, ref<gfx::texture>> tex_map{};
std::unordered_map<std::string, ref<gfx::model>>   model_map{};
} // anonymous namespace

void destroy()
{
    tex_map.clear();
    model_map.clear();
}

ref<gfx::texture> load_texture(const char* filename)
{
    const auto it = tex_map.find(filename);
    if (it != tex_map.end())
    {
        return it->second;
    }

    auto tex          = create_ref<gfx::texture>(filename);
    tex_map[filename] = tex;
    return tex;
}

ref<gfx::model> load_model(const char* filename)
{
    const auto it = model_map.find(filename);
    if (it != model_map.end())
    {
        return it->second;
    }

    auto mdl = create_ref<gfx::model>();
    mdl->init(filename);
    model_map[filename] = mdl;
    return mdl;
}


} // namespace yae::assets