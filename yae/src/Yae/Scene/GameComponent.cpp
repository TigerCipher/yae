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

using namespace DirectX;

namespace yae
{
model_component::model_component(const std::string_view filename)
{
    m_model = new gfx::model{};
    if (!m_model->init(filename))
    {
        LOG_ERROR("Failed to load model file {} from component", filename);
        // TODO: Throw exception?
    }
}

model_component::~model_component()
{
    if(m_model)
    {
        SAFE_DELETE(m_model);
    }
}

bool model_component::render(gfx::shader* shader)
{
    return m_model->render(shader, m_owner->world_transformation());
}


texture_component::texture_component(const char* filename)
{
    if (!m_texture.init(filename))
    {
        LOG_ERROR("Failed to load texture file {} from component", filename);
    }
}

bool texture_component::render(gfx::shader* shader)
{
    shader->set_texture(m_texture.texture_view());
    return true;
}

} // namespace yae