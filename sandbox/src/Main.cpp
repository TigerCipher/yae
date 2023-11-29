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

#include <random>

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


class shared_test
{
public:
    shared_test(u32 id) : m_id(id) {}
    ~shared_test() { LOG_DEBUG("Shared test destructor, id: {}", m_id); }

private:
    u32 m_id{};
};

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

    game_object m_lights{};
    game_object m_light_sphere{};

    //gfx::bitmap_font m_font{ };
    gfx::text_string       m_text{};
    ref<shared_test> m_t{};

public:
    ~sandbox() override = default;
    bool init() override
    {
        events::register_listener(events::app_quit, nullptr, on_app_quit);
        events::register_listener(events::key_pressed, nullptr, on_key_typed);

        //m_font.load_font("./assets/fonts/Coolvetica");
        m_text.init(gfx::fonts::coolvetica(), "Apple Monkey googly quack", 10, 500);

        ref<shared_test> t = create_ref<shared_test>(0);

        m_t = create_ref<shared_test>(1);
        m_t = nullptr;

        m_camera->set_position(0.f, 1.f, -12.f);
        m_camera->set_sensitivity(25.f);
        m_camera->set_speed(25.f);
        //m_cam.add(new camera_component{})->add(new move_component{})->add(new freelook_component{});
        //m_cam.set_position(0.f, 1.f, -12.f);
        //m_root.add(m_cam);
        gfx::material dirty_bricks{};
        dirty_bricks.diffuse = assets::load_texture("./assets/textures/bricks.tga")->texture_view();
        dirty_bricks.blend   = assets::load_texture("./assets/textures/dirt.tga")->texture_view();
        dirty_bricks.normal  = assets::load_texture("./assets/textures/bricks_normal.tga")->texture_view();

        gfx::material dirty_stone{};
        dirty_stone.diffuse = assets::load_texture("./assets/textures/stone01.tga")->texture_view();
        //dirty_stone.blend   = assets::load_texture("./assets/textures/dirt.tga")->texture_view();
        dirty_stone.normal  = assets::load_texture("./assets/textures/normal01.tga")->texture_view();

        gfx::material bricks{};
        bricks.diffuse = assets::load_texture("./assets/textures/bricks.tga")->texture_view();
        bricks.tint    = { 0.6f, 0.f, 0.3f, 1.f };
        bricks.normal = assets::load_texture("./assets/textures/bricks_normal.tga")->texture_view();

        //m_cube.add(new model_component{ gfx::geometry::create_box(3.f, 3.f, 1.f) })->set_material(dirty_bricks);
        m_cube.add(new model_component{"./assets/models/cube.txt"})->set_material(dirty_stone);
        m_cube.set_scale(3.f, 3.f, 1.f);
        m_cube.set_position(73.f, 6.f, 73.f);
        //m_cube.rotate(180.f, axis::z);

        game_object* sphere = new game_object{};
        sphere->add(new model_component{"./assets/models/sphere.txt"})->set_material(dirty_bricks);
        sphere->set_position(3.f, 9.f, 6.f);
        m_root.add(sphere);

        //m_cube2.add(new model_component{ gfx::geometry::create_box(1.f, 1.f, 1.f) });
        m_cube2.add(new model_component{ "./assets/models/cube.txt" });
        m_ball1.add(new model_component{ gfx::geometry::create_sphere(1.5f, 36, 36) });
        m_ball1.add(m_cube2);
        m_cube2.set_position(3.f, 1.f, 0.f);
        m_cube2.rotate(45.f, axis::x);
        m_cube2.set_material(dirty_stone);
        m_ball1.set_material(bricks);

        m_ball1.set_position(-3.f, 5.f, -3.f);

        //m_plane.add(new texture_component{"./assets/textures/stone01.tga"})->add(new model_component{gfx::geometry::create_plane(32, 32)});
        m_plane.add(new model_component{ "./assets/models/plane2.txt" })->set_material(dirty_stone);
        //m_plane.rotate(90.f, axis::x);
        m_plane.set_position(0.f, -1.f, 0.f);
        m_plane.set_scale(100.f, 100.f, 100.f);

        m_light_sphere.add(new pointlight_component{
            {0.f, 1.f, 0.f},
            10.f, 5.f, 4.f
        });
        m_light_sphere.set_position(3.f, 10.f, 3.f);
        m_lights.add(m_light_sphere);

        auto* light2 = new game_object{};
        light2->add(new pointlight_component{
            {1.f, 0.f, 0.f},
            10.f, 5.f, 4.f
        });
        light2->set_position(-5.f, 2.f, 0.f);
        m_lights.add(light2);

        std::random_device                  rd{};
        std::mt19937                        gen(rd());
        std::uniform_real_distribution<f32> dist(0.2f, 1.0f);
        u32                                 count = 0;
        for (i32 x = -40; x < 50; x += 20)
        {
            for (i32 z = -40; z < 50; z += 20)
            {
                LOG_DEBUG("Adding light {}", ++count);
                auto* new_light = new game_object{};
                f32   r         = dist(gen);
                f32   g         = dist(gen);
                f32   b         = dist(gen);
                new_light->add(new pointlight_component{
                    {r, g, b},
                    30.f, 25.5f, 22.f
                });
                new_light->set_position((f32) x, 1.f, (f32) z);
                m_lights.add(new_light);
            }
        }

        //m_root.add(m_light_sphere);
        //m_quad.add(new bitmap_component{ 150, 150, "./assets/textures/default.tga" });
        //m_quad.set_position(-1920.f / 2.f, -1080.f / 2.f, 0);


        m_root.add(m_cube);
        m_root.add(m_ball1);
        m_root.add(m_plane);

        return true;
    }

    void process_input(f32 delta)
    {
        if (input::key_down('W'))
        {
            m_camera->move(0, 0, delta);
        }
        if (input::key_down('A'))
        {
            m_camera->move(-delta, 0, 0);
        }
        if (input::key_down('S'))
        {
            m_camera->move(0, 0, -delta);
        }
        if (input::key_down('D'))
        {
            m_camera->move(delta, 0, 0);
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

        //m_cube2.rotate(rotation, axis::y);
        m_ball1.transformation().rotate(rotation, axis::y, true);

        //m_quad.rotate(rotation, axis::z);

        m_root.update(delta);
        //m_quad.update(delta);
        //m_lights.update(delta);
    }

    bool render() override
    {
        if (!m_root.render())
        {
            return false;
        }
        return true;
    }

    bool render2d() override
    {
        //m_quad.render();

        //m_text.draw();
        return true;
    }


    void shutdown() override {}
};


game* create_game()
{
    return new sandbox{};
}

void pre_init()
{
    g_settings->set("display", "fullscreen", false);
    g_settings->set("window", "width", 1920);
    g_settings->set("window", "height", 1080);
    g_settings->set("display", "vsync", false);
}