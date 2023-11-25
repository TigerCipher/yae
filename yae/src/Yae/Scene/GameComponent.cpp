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
//  File Name: GameComponent.cpp
//  Date File Created: 11/16/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#include "GameComponent.h"

#include "Yae/Graphics/Renderer.h"
#include "Yae/Graphics/Geometry.h"

using namespace DirectX;

namespace yae
{
model_component::model_component(const std::string_view filename, const char* tex_file, const char* blendtex)
{
    m_model = assets::load_model(filename.data());
    m_texture = assets::load_texture(tex_file);
    if(blendtex)
    {
        m_blendtex = assets::load_texture(blendtex);
    }
}

model_component::~model_component()
{
    if (m_model_managed)
    {
        SAFE_DELETE(m_model);
    }
}

bool model_component::render()
{
    gfx::render3d(m_model, m_texture, m_owner->world_transformation(), m_blendtex);
    return true;
}

bitmap_component::bitmap_component(u32 width, u32 height, const char* filename)
{
    m_texture = assets::load_texture(filename);
    if (!m_texture)
    {
        LOG_ERROR("Failed to load texture file {} from bitmap component", filename);
        return;
    }
    m_model = gfx::geometry::create_quad(width, height);
}

bitmap_component::~bitmap_component()
{
    if (m_model)
    {
        SAFE_DELETE(m_model);
    }
}

bool bitmap_component::render()
{
    gfx::render2d(m_model, m_texture, m_owner->world_transformation());
    return true;
}


bool pointlight_component::render()
{
    // rendering now handled by renderer.cpp
    //gfx::render_pointlight(m_owner->position(), m_owner->world_transformation(), m_light_color, m_radius, m_intensity, m_falloff);
    return true;
}

void pointlight_component::add_to_engine()
{
    gfx::add_pointlight(m_owner, this);
}

} // namespace yae