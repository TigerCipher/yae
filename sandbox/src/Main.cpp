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

#include "Yae/Graphics/Geometry.h"


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

    game_object m_root{};
    game_object m_cube{};
    game_object m_cube2{};
    game_object m_ball1{};

    game_object m_quad{};
    game_object m_plane{};
    game_object m_cam{};
    game_object m_light_sphere{};


public:
    ~sandbox() override = default;
    bool init() override
    {
        events::register_listener(events::app_quit, nullptr, on_app_quit);
        events::register_listener(events::key_pressed, nullptr, on_key_typed);

        m_camera->set_position(0.f, 1.f, -12.f);
        m_camera->set_sensitivity(25.f);
        m_camera->set_speed(25.f);
        //m_cam.add(new camera_component{})->add(new move_component{})->add(new freelook_component{});
        //m_cam.set_position(0.f, 1.f, -12.f);
        //m_root.add(m_cam);

        m_cube.add(new model_component{ gfx::geometry::create_box(1.f, 1.f, 1.f), "./assets/textures/bricks.tga" });
        m_cube.set_position(3.f, 9.f, 3.f);

        m_cube2.add(new model_component{gfx::geometry::create_box(1.f, 1.f, 1.f), "./assets/textures/default.tga"});
        m_ball1.add(new model_component{gfx::geometry::create_sphere(1.5f, 36, 36), "./assets/textures/bricks.tga"});
        m_ball1.add(m_cube2);
        m_cube2.set_position(3.f, 1.f, 0.f);
        m_cube2.rotate(45.f, axis::x);

        m_ball1.set_position(-3.f, 5.f, -3.f);

        //m_plane.add(new texture_component{"./assets/textures/stone01.tga"})->add(new model_component{gfx::geometry::create_plane(32, 32)});
        m_plane.add(new model_component{"./assets/models/plane.txt", "./assets/textures/stone01.tga"});
        //m_plane.rotate(90.f, axis::x);
        m_plane.set_position(0.f, -1.f, 0.f);
        m_plane.set_scale(10.f, 1.f, 10.f);

        m_light_sphere.add(new model_component{ gfx::geometry::create_sphere(0.5f), "./assets/textures/default.tga" });
        m_light_sphere.set_position(3.f, 10.f, 3.f);

        m_root.add(m_light_sphere);


        m_quad.add(new bitmap_component{150, 150, "./assets/textures/default.tga"});
        m_quad.set_position(-1920.f / 2.f, -1080.f / 2.f, 0);



        m_root.add(m_cube);
        m_root.add(m_ball1);
        m_root.add(m_plane);

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

        m_cube2.rotate(rotation, axis::y); // TODO: Update on parent transformation update as well
        m_ball1.transformation().rotate(rotation, axis::y, true);

        m_quad.rotate(rotation, axis::z);

        m_root.update(delta);
        m_quad.update(delta);
    }

    bool render() override
    {
        if(!m_root.render())
        {
            return false;
        }
        return true;
    }

    bool render2d() override
    {
        m_quad.render();

        return true;
    }

    void shutdown() override
    {
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