// ------------------------------------------------------------------------------
//
// yae
//    Copyright 2023 Matthew Rogers
//
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.
//
// File Name: Main
// Date File Created: 11/11/2023
// Author: Matt
//
// ------------------------------------------------------------------------------

#include <Yae/Entrypoint.h>

const char* game_name    = "sandbox";
const char* game_version = "1.0.0";

using namespace yae;
using namespace DirectX;

class sandbox : public game
{
private:
    gfx::model*        m_model{};
    gfx::light_shader* m_lights_shader{};
    gfx::texture*      m_bricks_texture{};
    gfx::base_light    m_light{};

public:
    ~sandbox() override = default;
    bool init() override
    {
        m_camera->set_position(0.f, 0.f, -10.f);
        {
            gfx::shader_layout layout{};
            layout.add<math::vec3>("POSITION");
            layout.add<math::vec2>("TEXCOORD");
            layout.add<math::vec3>("NORMAL");
            m_lights_shader = gfx::create_shader<gfx::light_shader>("Light", layout);
            if (!m_lights_shader)
            {
                popup::show("Failed to initialize the test shader", "Error", popup::style::error);
                return false;
            }
        }

        m_model = DBG_NEW gfx::model{};
        if (!m_model->init("./assets/models/sphere.txt"))
        {
            popup::show("Failed to initialize the test model", "Error", popup::style::error);
            return false;
        }
        //std::vector<gfx::vertex_position_texture_normal> verts{ 3 };
        //std::vector<u32>                                 indices(3);

        //// bottom left
        //verts[0].position = { -1.f, -1.f, 0.f };
        //verts[0].texture  = { 0.f, 1.f };
        //verts[0].normal   = { 0.f, 0.f, -1.f };

        //// top middle
        //verts[1].position = { 0.f, 1.f, 0.f };
        //verts[1].texture  = { 0.5f, 0.f };
        //verts[1].normal   = { 0.f, 0.f, -1.f };

        //// bottom right
        //verts[2].position = { 1.f, -1.f, 0.f };
        //verts[2].texture  = { 1.f, 1.f };
        //verts[2].normal   = { 0.f, 0.f, -1.f };

        //indices[0] = 0; // Bottom left
        //indices[1] = 1; // Top middle
        //indices[2] = 2; // Bottom right
        //if (!m_model->init(verts, indices))
        //{
        //    popup::show("Failed to initialize the test model", "Error", popup::style::error);
        //    return false;
        //}

        m_bricks_texture = DBG_NEW gfx::texture{};
        if (!m_bricks_texture->init("./assets/textures/bricks.tga"))
        {
            popup::show("Failed to initialize the test texture", "Error", popup::style::error);
            return false;
        }

        m_light.ambient_color  = { 0.15f, 0.15f, 0.15f, 1.f };
        m_light.diffuse_color  = { 1.f, 1.f, 1.f, 1.f };
        m_light.direction      = { 1.f, 0.f, 1.f };
        m_light.specular_color = { 1.f, 1.f, 1.f, 1.f };
        m_light.specular_power = 32.f;

        return true;
    }
    bool render() override
    {
        static f32 rotation{};
        rotation -= math::deg2rad_multiplier * 0.1f;
        if (rotation < 0.f)
        {
            rotation += 360.f;
        }

        math::matrix rotate    = XMMatrixRotationY(rotation);
        math::matrix translate = XMMatrixTranslation(-2.f, 0.f, 0.f);
        gfx::core::set_world_matrix(XMMatrixMultiply(rotate, translate));

        m_model->render();

        m_lights_shader->set_texture(m_bricks_texture->texture_view());
        m_lights_shader->set_camera(m_camera);
        m_lights_shader->set_light(&m_light);
        if (!m_lights_shader->render(m_model->index_count()))
        {
            return false;
        }

        math::matrix scale = XMMatrixScaling(0.5f, 0.5f, 0.5f);
        rotate             = XMMatrixRotationX(rotation);
        translate          = XMMatrixTranslation(2.f, 0.f, 0.f);
        gfx::core::set_world_matrix(XMMatrixMultiply(XMMatrixMultiply(scale, rotate), translate));

        m_model->render();
        if (!m_lights_shader->render(m_model->index_count()))
        {
            return false;
        }

        return true;
    }

    void shutdown() override
    {
        gfx::core::shutdown(m_bricks_texture);
        gfx::core::shutdown(m_lights_shader);
        gfx::core::shutdown(m_model);
    }
};


game* create_game()
{
    return new sandbox{};
}

void pre_init()
{
    g_settings->set("display", "fullscreen", false);
}