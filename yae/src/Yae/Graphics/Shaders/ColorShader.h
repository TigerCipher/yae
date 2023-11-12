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
// File Name: ColorShader
// Date File Created: 11/12/2023
// Author: Matt
//
// ------------------------------------------------------------------------------
#pragma once

#include "../D3D11Common.h"

namespace yae::gfx
{

class color_shader
{
public:
    color_shader()  = default;
    ~color_shader() = default;

    bool init();
    void shutdown();
    bool render(i32 index_count, const math::matrix& view) const;

private:

    bool set_parameters(const math::matrix& view) const;

    struct matrix_buffer
    {
        math::matrix world;
        math::matrix view;
        math::matrix projection;
    };

    ID3D11VertexShader* m_vertex_shader{};
    ID3D11PixelShader*  m_pixel_shader{};
    ID3D11InputLayout*  m_layout{};
    ID3D11Buffer*       m_matrix_buffer{};
};

} // namespace yae::gfx
