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
//  File Name: Texture.h
//  Date File Created: 11/13/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#pragma once

#include "D3D11Common.h"

namespace yae::gfx
{

class texture
{
public:
    texture()  = default;
    ~texture() = default;

    bool init(const char* filename);
    void shutdown();

    constexpr ID3D11ShaderResourceView* texture_view() const { return m_texture_view; }

    constexpr u32 width() const { return m_width; }
    constexpr u32 height() const { return m_height; }

private:
    struct targa_header
    {
        u8  data1[12];
        u16 width;
        u16 height;
        u8  bpp;
        u8  data2;
    };

    bool load_targa_32bit(const char* filename);

    std::vector<u8>           m_data{};
    ID3D11Texture2D*          m_texture{};
    ID3D11ShaderResourceView* m_texture_view{};
    u32                       m_width{};
    u32                       m_height{};
};

} // namespace yae::gfx