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
//  File Name: Model.h
//  Date File Created: 11/12/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#pragma once

#include "D3D11Common.h"

namespace yae::gfx
{

class model
{
public:
    model() = default;
    ~model() = default;

    bool init();
    void shutdown();
    void render();

    constexpr u32 index_count() const { return m_index_count; }
private:
    struct vertex
    {
        math::vec3 position{};
        math::vec4 color{};
    };

    ID3D11Buffer* m_vertex_buffer{};
    ID3D11Buffer* m_index_buffer{};
    u32 m_vertex_count{};
    u32 m_index_count{};
};

}