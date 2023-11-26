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
shader_obj dr{};
shader_obj dir{};
shader_obj fonts{};
shader_obj multitex{};
} // anonymous namespace

bool init()
{
    tex.vs      = new vertex_shader{};
    tex.ps      = new pixel_shader{};
    lights.vs   = new vertex_shader{};
    lights.ps   = new pixel_shader{};
    dr.vs       = new vertex_shader{};
    dr.ps       = new pixel_shader{};
    dir.vs      = new vertex_shader{};
    dir.ps      = new pixel_shader{};
    fonts.vs    = new vertex_shader{};
    fonts.ps    = new pixel_shader{};
    multitex.vs = dr.vs;
    multitex.ps = new pixel_shader{};

#ifdef _DEBUG
    std::string firstbit = "../bin/Debug/";
#else
    std::string firstbit = "../bin/Release/";
#endif

    if (!tex.vs->load_from_file(firstbit + "TextureVertexShader.cso"))
    {
        shutdown();
        return false;
    }

    if (!tex.ps->load_from_file(firstbit + "TexturePixelShader.cso"))
    {
        shutdown();
        return false;
    }

    if (!lights.vs->load_from_file(firstbit + "LightVertexShader.cso"))
    {
        shutdown();
        return false;
    }

    if (!lights.ps->load_from_file(firstbit + "LightPixelShader.cso"))
    {
        shutdown();
        return false;
    }

    if (!dr.vs->load_from_file(firstbit + "DeferredVertexShader.cso"))
    {
        shutdown();
        return false;
    }

    if (!dr.ps->load_from_file(firstbit + "DeferredPixelShader.cso"))
    {
        shutdown();
        return false;
    }

    if (!dir.vs->load_from_file(firstbit + "DirectionLightVertexShader.cso"))
    {
        shutdown();
        return false;
    }

    if (!dir.ps->load_from_file(firstbit + "DirectionLightPixelShader.cso"))
    {
        shutdown();
        return false;
    }

    if (!fonts.vs->load_from_file(firstbit + "FontVertexShader.cso"))
    {
        shutdown();
        return false;
    }

    if (!fonts.ps->load_from_file(firstbit + "FontPixelShader.cso"))
    {
        shutdown();
        return false;
    }

    if (!multitex.ps->load_from_file(firstbit + "MultitexturePixelShader.cso"))
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
    SAFE_DELETE(dr.vs);
    SAFE_DELETE(dr.ps);
    SAFE_DELETE(dir.vs);
    SAFE_DELETE(dir.ps);
    SAFE_DELETE(fonts.vs);
    SAFE_DELETE(fonts.ps);
    SAFE_DELETE(multitex.ps);
}

shader_obj* texture_shader()
{
    return &tex;
}
//shader_obj* light_shader()
//{
//    return &lights;
//}

shader_obj* deferred()
{
    return &dr;
}

shader_obj* directional_light()
{
    return &dir;
}

shader_obj* lighting()
{
    return &lights;
}

shader_obj* font()
{
    return &fonts;
}

shader_obj* multitexture()
{
    return &multitex;
}

} // namespace yae::gfx::shaders