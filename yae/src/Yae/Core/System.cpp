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
//  File Name: System.cpp
//  Date File Created: 11/11/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------
#include "System.h"

#include "Input.h"
#include "Application.h"

namespace yae::system
{
namespace
{


LPCWSTR   application_name;
HINSTANCE hinstance;
HWND      hwnd;

application* app;

std::set<resolution> resolutions;


bool frame()
{
    if (input::is_key_down(VK_ESCAPE))
    {
        return false;
    }
    return app->frame();
}
void init_windows(i32& width, i32& height)
{
    LOG_INFO("Creating window");
    WNDCLASSEX wcex;
    DEVMODE    dm_screen_settings;
    i32        pos_x;
    i32        pos_y;

    application_name = L"YAE";

    hinstance = GetModuleHandle(nullptr);

    wcex.cbSize        = sizeof(WNDCLASSEX);
    wcex.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wcex.lpfnWndProc   = window_proc;
    wcex.cbClsExtra    = 0;
    wcex.cbWndExtra    = 0;
    wcex.hInstance     = hinstance;
    wcex.hIcon         = LoadIcon(nullptr, IDI_WINLOGO);
    wcex.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName  = nullptr;
    wcex.lpszClassName = application_name;
    wcex.hIconSm       = wcex.hIcon;

    RegisterClassEx(&wcex);

    MONITORINFO mi;
    mi.cbSize = sizeof(mi);

    HMONITOR hmonitor = MonitorFromWindow(nullptr, MONITOR_DEFAULTTOPRIMARY);
    GetMonitorInfo(hmonitor, &mi);

    i32 mode = 0;
    while (EnumDisplaySettings(nullptr, mode, &dm_screen_settings))
    {
        resolution res{ (i32) dm_screen_settings.dmPelsWidth, (i32) dm_screen_settings.dmPelsHeight };
        resolutions.insert(res);
        mode++;
    }

    width  = g_settings->get<i32>("display", "width");
    height = g_settings->get<i32>("display", "height");
    if (width == 0 || height == 0)
    {
        width  = resolutions.rbegin()->width;
        height = resolutions.rbegin()->height;

        g_settings->set("display", "width", width);
        g_settings->set("display", "height", height);
    }


    //width  = GetSystemMetrics(SM_CXSCREEN);
    //height = GetSystemMetrics(SM_CYSCREEN);

    if (g_settings->get<bool>("display", "fullscreen"))
    {
        memset(&dm_screen_settings, 0, sizeof(dm_screen_settings));
        dm_screen_settings.dmSize       = sizeof(dm_screen_settings);
        dm_screen_settings.dmPelsWidth  = (u32) width;
        dm_screen_settings.dmPelsHeight = (u32) height;
        dm_screen_settings.dmBitsPerPel = 32;
        dm_screen_settings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

        ChangeDisplaySettings(&dm_screen_settings, CDS_FULLSCREEN);

        pos_x = pos_y = 0;
    } else
    {
        width  = g_settings->get<i32>("window", "width");
        height = g_settings->get<i32>("window", "height");
        if (width == 0 || height == 0)
        {
            width  = 1280;
            height = 720;

            g_settings->set("window", "width", width);
            g_settings->set("window", "height", height);
        }

        pos_x = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
        pos_y = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;
    }

    LOG_INFO("Determined window size to create: ({}, {}). Fullscreen: {}. Position: ({}, {})", width, height,
             g_settings->get<bool>("display", "fullscreen"), pos_x, pos_y);

    hwnd = CreateWindowEx(WS_EX_APPWINDOW, application_name, application_name, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
                          pos_x, pos_y, width, height, nullptr, nullptr, hinstance, nullptr);

    ShowWindow(hwnd, SW_SHOW);
    SetForegroundWindow(hwnd);
    SetFocus(hwnd);

    ShowCursor(false);

    LOG_INFO("Window created and initialized");
}

void shutdown_windows()
{
    LOG_INFO("Destroying window");
    ShowCursor(true);
    if (g_settings->get<bool>("display", "fullscreen"))
    {
        ChangeDisplaySettings(nullptr, 0);
    }

    DestroyWindow(hwnd);
    hwnd = nullptr;

    UnregisterClass(application_name, hinstance);
    hinstance = nullptr;
}

} // anonymous namespace

bool init(game* game)
{
    LOG_INFO("Initializing YAE system");
    i32  screen_width  = 0;
    i32  screen_height = 0;
    bool result{};

    init_windows(screen_width, screen_height);

    LOG_INFO("Creating application");
    app = DBG_NEW application{ game };

    result = app->init(screen_width, screen_height, hwnd);

#ifdef _DEBUG
    if (result)
    {
        LOG_INFO("YAE system initialized");
    }
#endif
    return result;
}

void shutdown()
{
    LOG_INFO("Shutting down YAE system");
    if (app)
    {
        app->shutdown();
        delete app;
        app = nullptr;
    }

    shutdown_windows();

    LOG_INFO("YAE system shutdown");
}

void run()
{
    MSG  msg;
    bool done{};
    bool result{};

    ZeroMemory(&msg, sizeof(MSG));

    LOG_TRACE("Entering run loop");
    while (!done)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT)
            {
                done = true;
            }
        } else
        {
            result = frame();
            if (!result)
            {
                done = true;
            }
        }
    }
}


LRESULT message_handler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
    switch (umsg)
    {
    case WM_KEYDOWN: input::process_key((u32) wparam, true); return 0;
    case WM_KEYUP: input::process_key((u32) wparam, false); return 0;
    case WM_CHAR:
    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_MOUSEWHEEL:
    case WM_MOUSEHWHEEL:
    default: break;
    }

    return DefWindowProc(hwnd, umsg, wparam, lparam);
}

LRESULT window_proc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
    switch (umsg)
    {
    case WM_DESTROY:
    case WM_CLOSE: PostQuitMessage(0); return 0;
    default: return message_handler(hwnd, umsg, wparam, lparam);
    }
}

const std::set<resolution>& get_resolutions()
{
    return resolutions;
}

HWND handle()
{
    return hwnd;
}

} // namespace yae::system