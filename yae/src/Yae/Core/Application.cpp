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
//  File Name: Application.cpp
//  Date File Created: 11/12/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------
#include "Application.h"


#include "Input.h"
#include "Yae/Graphics/D3D11Core.h"
#include "Yae/Graphics/Renderer.h"

namespace yae
{

namespace app
{
namespace
{
game* game_instance;
bool  game_initialized{};
} // anonymous namespace

void set(game* game)
{
    if (game_initialized)
    {
        LOG_WARN("Game instance is already initialized");
        return;
    }

    game_instance = game;
}
game* instance()
{
    return game_instance;
}

} // namespace app


bool application::init(i32 width, i32 height, HWND hwnd)
{
    LOG_INFO("Initializing application");
    if (!gfx::core::init(width, height, hwnd, g_settings->get<bool>("display", "fullscreen"), 1000.f, 0.3f))
    {
        popup::show("Failed to initialize DirectX", "Error", popup::style::error);
        return false;
    }

    gfx::create_default_view_matrix();
    m_camera = DBG_NEW gfx::camera{};

    app::set(m_game);
    m_game->set_camera(m_camera);

    gfx::fonts::init();

    LOG_INFO("Initializing game");
    if (!m_game->init())
    {
        return false;
    }
    LOG_INFO("Game initialized");

    m_fps_string.init(gfx::fonts::coolvetica(), "FPS: 0", 10, 10);
    m_fps_string.set_color(0.f, 1.f, 0.f);

    m_fps.start();

    LOG_INFO("Application initialized");
    m_timer.start();
    return true;
}

void application::shutdown()
{
    LOG_INFO("Shutting down application");
    gfx::fonts::unload();
    LOG_INFO("Shutting down game");
    m_game->shutdown();
    SAFE_DELETE(m_game);
    LOG_INFO("Game shutdown");

    SAFE_DELETE(m_camera);

    LOG_INFO("Application shutdown");
}

bool application::frame()
{
    update_fps();
    m_timer.frame();
    //m_camera->calculate_view();
    //m_camera->update(m_timer.frame_time());
    m_game->update(m_timer.frame_time());

    input::update(m_timer.frame_time());
    return render();
}

bool application::render()
{
    gfx::core::begin_scene(0.f, 0.f, 0.f, 1.f);
    gfx::core::clear_first_stage();

    if (!m_game->render())
    {
        return false;
    }

    gfx::core::clear_second_stage();

    gfx::render_directional_light();

    gfx::render_all_pointlights();


    gfx::core::disable_zbuffer();
    gfx::core::enable_alpha_blending();
    if (!m_game->render2d())
    {
        return false;
    }
    const std::string fps = std::format("FPS: {}", m_fps.fps());
    m_fps_string.draw(fps);
    //gfx::core::disable_alpha_blending();
    //gfx::core::enable_zbuffer();

    gfx::core::end_scene();

    return true;
}

void application::update_fps()
{
    m_fps.frame();

    const u32 fps = m_fps.fps();

    if (fps >= 60)
    {
        m_fps_string.set_color(0.f, 1.f, 0.f);
    }
    if (fps < 60)
    {
        m_fps_string.set_color(1.f, 1.f, 0.f);
    }
    if (fps < 30)
    {
        m_fps_string.set_color(1.f, 0.f, 0.f);
    }
}

} // namespace yae