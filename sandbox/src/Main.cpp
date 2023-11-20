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

namespace
{
bool on_app_quit(u16 code, void* sender, void* listener, void* userdata)
{
    LOG_INFO("App quit event received and responded to via callback!");
    return true;
}


bool on_key_typed(u16 code, void* sender, void* listener, void* userdata)
{
    u16 key = GET_KEY(userdata);
    if (key == input::key::f1)
    {
        LOG_DEBUG("F1 was typed");
    }

    LOG_DEBUG("Key typed: code={}, char={}", key, (char) key);

    return false;
}

} // anonymous namespace

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
    game_object        m_plane{};
    game_object        ball3{};
    game_object        m_box2{};

    //game_object m_hud{};
    gfx::bitmap m_hud{true};


public:
    ~sandbox() override = default;
    bool init() override
    {
        events::register_listener(events::app_quit, nullptr, on_app_quit);
        events::register_listener(events::key_pressed, nullptr, on_key_typed);
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
        m_camera->set_position(0.f, 1.f, -12.f);
        m_camera->set_sensitivity(15.f);
        //m_camera->set_rotation(30.f, 0.f, 0.f);

        //m_camera->add(new move_component{})->add(new freelook_component{});

        m_plane.add(new texture_component{ "./assets/textures/bricks.tga" })
            ->add(new model_component{ "./assets/models/plane.txt" });

        box.add(DBG_NEW texture_component{ "./assets/textures/bricks.tga" })
            ->add(DBG_NEW model_component{ "./assets/models/cube.txt" });
        box.set_position(2.f, 1.f, 0.f);
        box.set_scale(0.3f);
        box.rotate(15.f, axis::x);
        ball.add(DBG_NEW texture_component{ "./assets/textures/bricks.tga" })
            ->add(DBG_NEW model_component{ "./assets/models/sphere.txt" });
        ball.add(box);
        ball.set_position(-4.f, 0.f, 0.f);
        ball.set_scale(1.5f);

        ball2.add(DBG_NEW texture_component{ "./assets/textures/default.tga" })
            ->add(DBG_NEW model_component{ "./assets/models/sphere.txt" });
        ball2.set_position(2.f, 0.f, 0.f);
        ball2.set_scale(0.75f);

        ball3.add(DBG_NEW texture_component{ "./assets/textures/bricks.tga" })
            ->add(DBG_NEW model_component{ "./assets/models/cube.txt" })
            ->add(new move_component{});
        ball3.set_position(2.f, 2.f, 0.f);


        m_box2.add(new texture_component{ "./assets/textures/default.tga" })
            ->add(new model_component{ "./assets/models/cube.txt" });
        m_box2.set_position(0.f, 2.f, -2.f);
        m_box2.set_rotation(0.f, 45.f, 0.f);

        //m_hud.add(new bitmap_component{ "./assets/textures/stone01.tga", 50, 50 });
        m_hud.init("./assets/textures/stone01.tga", 50, 50);
        //m_hud.set_location(0, 0);

        //m_hudobj.set_position(3.f, 0.f, 0.f);
        //m_hudobj.rotate(45.f, axis::z);

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

    void process_input(f32 delta)
    {
        if (input::key_down('W'))
        {
            m_camera->move(0, 0, 1, delta);
        }
        if (input::key_down('A'))
        {
            m_camera->move(-1, 0, 0, delta);
        }
        if (input::key_down('S'))
        {
            m_camera->move(0, 0, -1, delta);
        }
        if (input::key_down('D'))
        {
            m_camera->move(1, 0, 0, delta);
        }

        if (input::button_pressed(input::button::left))
        {
            input::lock_cursor(true);
        } else if (input::button_pressed(input::button::right))
        {
            input::lock_cursor(false);
        }

        if (input::is_cursor_locked())
        {
            i32 x, y;
            input::get_mouse_position(&x, &y);
            math::vec2 pos       = { (f32) x, (f32) y };
            math::vec2 center    = { (f32) system::width() / 2, (f32) system::height() / 2 };
            math::vec2 delta_pos = { pos.x - center.x, pos.y - center.y };
            m_camera->rotate(delta_pos.x, delta_pos.y, delta);
            input::center_cursor();
        }
    }

    void update(f32 delta) override
    {
        process_input(delta);
        if (input::key_released('B'))
        {
            LOG_DEBUG("Frame time: {}", delta);
        }
        const f32 rotation = -15.f * delta;

        ball.rotate(rotation, axis::y);
        box.rotate(rotation, axis::x);
        ball2.rotate(rotation, axis::x);
        //m_box2.rotate(rotation, m_box2.transformation().right());
        //m_box2.transformation().rotate(rotation, {1.f, 0.f, 0.f, 0.f}, true);
        m_box2.transformation().rotate(rotation, m_box2.transformation().right(), true);

        m_plane.update(delta);
        ball.update(delta);
        ball2.update(delta);
        ball3.update(delta);
        m_box2.update(delta);
    }

    bool render() override
    {
        m_light.specular_power = 32.f;
        m_light.specular_color = { 1.f, 1.f, 1.f, 1.f };

        if (!m_plane.render(m_lights_shader))
        {
            return false;
        }


        if (!ball.render(m_lights_shader))
        {
            return false;
        }

        if (!ball3.render(m_lights_shader))
        {
            return false;
        }

        if (!m_box2.render(m_lights_shader))
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

    bool render2d() override
    {
        if (!m_hud.render(m_texture_shader, XMMatrixIdentity()))
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