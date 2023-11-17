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
    gfx::shader*       m_texture_shader{};
    gfx::light_shader* m_lights_shader{};
    gfx::base_light    m_light{};
    gfx::point_light   m_point_lights[4]{};
    game_object        ball{};
    game_object        ball2{};
    game_object        box{};
    game_object m_plane{};

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
                popup::show("Failed to initialize the lights shader", "Error", popup::style::error);
                return false;
            }
        }

        {
            gfx::shader_layout layout{};
            layout.add<math::vec3>("POSITION");
            layout.add<math::vec2>("TEXCOORD");
            m_texture_shader = gfx::create_shader<gfx::shader>("Texture", layout);
            if (!m_texture_shader)
            {
                popup::show("Failed to initialize the texture shader", "Error", popup::style::error);
                return false;
            }
        }
        m_camera->set_position(0.f, 7.f, -12.f);
        m_camera->set_rotation(30.f, 0.f, 0.f);
        m_camera->render();

        m_plane.add(new texture_component{"./assets/textures/bricks.tga"})->add(new model_component{"./assets/models/plane.txt"});

        box.add(DBG_NEW texture_component{ "./assets/textures/bricks.tga" })
            ->add(DBG_NEW model_component{ "./assets/models/cube.txt" });
        box.set_position(2.f, 1.f, 0.f);
        box.set_scale(0.3f);
        box.set_rotation(math::deg2rad_multiplier * 15.f, axis::x);
        ball.add(DBG_NEW texture_component{ "./assets/textures/bricks.tga" })
            ->add(DBG_NEW model_component{ "./assets/models/sphere.txt" });
        ball.add(box);
        ball.set_position(-4.f, 0.f, 0.f);
        ball.set_scale(1.5f);

        ball2.add(DBG_NEW texture_component{ "./assets/textures/default.tga" })
            ->add(DBG_NEW model_component{ "./assets/models/sphere.txt" });
        ball2.set_position(2.f, 0.f, 0.f);
        ball2.set_scale(0.75f);

        m_light.ambient_color  = { 0.15f, 0.15f, 0.15f, 1.f };
        m_light.diffuse_color  = { 1.f, 1.f, 1.f, 1.f };
        m_light.direction      = { 1.f, 0.f, 1.f };
        m_light.specular_color = { 1.f, 1.f, 1.f, 1.f };
        m_light.specular_power = 32.f;


        gfx::point_light light1{};
        light1.position         = { -3.f, 1.f, 3.f };
        light1.diffuse_color    = { 1.f, 0.f, 0.f, 1.f };
        light1.constant_factor  = 3.f;
        light1.linear_factor    = 0.09f;
        light1.quadradic_factor = 0.032f;

        gfx::point_light light2{};
        light2.position         = { 3.f, 1.f, 3.f };
        light2.diffuse_color    = { 0.f, 1.f, 0.f, 1.f };
        light2.constant_factor  = 1.f;
        light2.linear_factor    = 0.09f;
        light2.quadradic_factor = 0.032f;

        gfx::point_light light3{};
        light3.position         = { -3.f, 1.f, -3.f };
        light3.diffuse_color    = { 0.f, 0.f, 1.f, 1.f };
        light3.constant_factor  = 2.f;
        light3.linear_factor    = 0.09f;
        light3.quadradic_factor = 0.032f;

        gfx::point_light light4{};
        light4.position         = { 3.f, 1.f, -3.f };
        light4.diffuse_color    = { 1.f, 1.f, 0.f, 1.f }; // set alpha = 0 to make this light not get applied
        light4.constant_factor  = 2.f;
        light4.linear_factor    = 0.09f;
        light4.quadradic_factor = 0.032f;

        m_point_lights[0] = light1;
        m_point_lights[1] = light2;
        m_point_lights[2] = light3;
        m_point_lights[3] = light4;

        //m_lights_shader->set_camera(m_camera); // by default, shader will now set the camera initially when init is called

        m_lights_shader->set_point_lights(m_point_lights);
        m_lights_shader->set_light(&m_light);
        return true;
    }

    void update(f32 delta) override
    {
        if (input::is_key_down('B'))
        {
            LOG_DEBUG("Frame time: {}", delta);
        }
        static f32 rotation{};
        rotation -= math::deg2rad_multiplier * 15.f * delta;
        if (rotation < 0.f)
        {
            rotation += 360.f;
        }

        ball.set_rotation(rotation, axis::y);
        box.set_rotation(rotation, axis::x);
        ball2.set_rotation(rotation, axis::x);

        m_plane.update(delta);
        ball.update(delta);
        ball2.update(delta);
    }

    bool render() override
    {
        m_light.specular_power = 32.f;
        m_light.specular_color = { 1.f, 1.f, 1.f, 1.f };

        if(!m_plane.render(m_lights_shader))
        {
            return false;
        }


        if (!ball.render(m_lights_shader))
        {
            return false;
        }


        m_light.specular_power = 20.f;
        m_light.specular_color = { 0.8f, 0.2f, 0.2f, 1.f };

        if (!ball2.render(m_lights_shader))
        {
            return false;
        }

        return true;
    }

    void shutdown() override
    {
        gfx::core::shutdown(m_lights_shader);
        gfx::core::shutdown(m_texture_shader);
    }
};


game* create_game()
{
    return DBG_NEW sandbox{};
}

void pre_init()
{
    g_settings->set("display", "fullscreen", false);
    g_settings->set("window", "width", 1920);
    g_settings->set("window", "height", 1080);
    g_settings->set("display", "vsync", false);
}