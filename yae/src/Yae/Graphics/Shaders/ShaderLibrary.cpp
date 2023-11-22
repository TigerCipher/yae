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
//  File Name: ShaderLibrary.cpp
//  Date File Created: 11/22/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#include "ShaderLibrary.h"

namespace yae::gfx::shaders
{

namespace
{
shader_obj tex{};
shader_obj lights{};

} // anonymous namespace

bool init()
{
    tex.vs    = new vertex_shader{};
    tex.ps    = new pixel_shader{};
    lights.vs = new vertex_shader{};
    lights.ps = new pixel_shader{};

    if (!tex.vs->load_from_file("../bin/Debug/TextureVertexShader.cso"))
    {
        shutdown();
        return false;
    }

    if (!tex.ps->load_from_file("../bin/Debug/TexturePixelShader.cso"))
    {
        shutdown();
        return false;
    }

    if (!lights.vs->load_from_file("../bin/Debug/LightsVertexShader.cso"))
    {
        shutdown();
        return false;
    }

    if (!lights.ps->load_from_file("../bin/Debug/LightsPixelShader.cso"))
    {
        shutdown();
        return false;
    }

    return true;
}

void shutdown()
{
    SAFE_DELETE(tex.vs);
    SAFE_DELETE(tex.ps);
    SAFE_DELETE(lights.vs);
    SAFE_DELETE(lights.ps);
}

shader_obj* texture_shader()
{
    return &tex;
}
shader_obj* light_shader()
{
    return &lights;
}

} // namespace yae::gfx::shaders