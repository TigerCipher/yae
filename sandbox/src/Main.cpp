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

#include <Yae/Yae.h>

#include <Windows.h>
#include <filesystem>
#include <fstream>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    yae::init("sandbox");
    yae::g_settings->set("display", "fullscreen", false);


    bool result = yae::system::init();

    //for (const auto [width, height] : yae::system::get_resolutions())
    //{
    //    OutputDebugStringA("Width: ");
    //    OutputDebugStringA(std::to_string(width).c_str());
    //    OutputDebugStringA(", Height: ");
    //    OutputDebugStringA(std::to_string(height).c_str());
    //    OutputDebugStringA("\n");
    //}

    if (result)
    {
        yae::system::run();
    }

    yae::system::shutdown();
    yae::shutdown();
    return 0;
}