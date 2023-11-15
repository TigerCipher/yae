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

class sandbox : public game
{
private:
    gfx::model*   m_model{};
    gfx::shader*  m_test_color_shader{};
    gfx::shader*  m_test_texture_shader{};
    gfx::texture* m_bricks_texture{};

public:
    ~sandbox() override = default;
    bool init() override
    {
        {
            gfx::shader_layout layout{};
            layout.add<math::vec3>("POSITION");
            layout.add<math::vec4>("COLOR");
            m_test_color_shader = gfx::create_shader<gfx::shader>("Color", layout);
            if (!m_test_color_shader)
            {
                MessageBox(nullptr, L"Failed to initialize the test shader", L"Error", MB_OK);
                return false;
            }
        }

        {
            gfx::shader_layout layout{};
            layout.add<math::vec3>("POSITION");
            layout.add<math::vec2>("TEXCOORD");
            m_test_texture_shader = gfx::create_shader<gfx::shader>("Texture", layout);
            if (!m_test_texture_shader)
            {
                MessageBox(nullptr, L"Failed to initialize the test texture shader", L"Error", MB_OK);
                return false;
            }
        }

        m_model = new gfx::model{};
        std::vector<gfx::vertex_position_texture> verts{ 3 };
        std::vector<u32>                          indices(3);

        // bottom left
        verts[0].position = { -1.f, -1.f, 0.f };
        verts[0].texture  = { 0.f, 1.f };

        // top middle
        verts[1].position = { 0.f, 1.f, 0.f };
        verts[1].texture  = { 0.5f, 0.f };

        // bottom right
        verts[2].position = { 1.f, -1.f, 0.f };
        verts[2].texture  = { 1.f, 1.f };

        indices[0] = 0; // Bottom left
        indices[1] = 1; // Top middle
        indices[2] = 2; // Bottom right
        if (!m_model->init(verts, indices))
        {
            MessageBox(nullptr, L"Failed to initialize the test model", L"Error", MB_OK);
            return false;
        }

        m_bricks_texture = new gfx::texture{};
        if (!m_bricks_texture->init("./assets/textures/bricks.tga"))
        {
            MessageBox(nullptr, L"Failed to initialize the texture", L"Error", MB_OK);
            return false;
        }

        return true;
    }
    bool render() override
    {
        m_model->render();


        m_test_texture_shader->set_parameters({ [&] { gfx::shader::set_texture(m_bricks_texture->texture_view()); } });
        if (!m_test_texture_shader->render(m_model->index_count(), m_camera->view()))
        {
            return false;
        }
        return true;
    }

    void shutdown() override
    {
        gfx::core::shutdown(m_bricks_texture);
        gfx::core::shutdown(m_test_texture_shader);
        gfx::core::shutdown(m_test_color_shader);
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