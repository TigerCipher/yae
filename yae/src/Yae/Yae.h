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
// File Name: Yae
// Date File Created: 11/11/2023
// Author: Matt
//
// ------------------------------------------------------------------------------
#pragma once

#include "Common.h"
#include "Globals.h"

// Core
#include "Core/System.h"
#include "Core/Application.h"
#include "Core/Game.h"

// Util
#include "Util/PathUtil.h"

// Graphics
#include "Graphics/D3D11Common.h"
#include "Graphics/D3D11Core.h"
#include "Graphics/Light.h"
#include "Graphics/Model.h"
#include "Graphics/Vertex.h"
#include "Graphics/Shaders/Shader.h"
#include "Graphics/Shaders/LightShader.h"

namespace yae
{
void init(const std::string& game_name, const std::string& version);
void shutdown();
}