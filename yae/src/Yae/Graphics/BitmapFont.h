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
//  File Name: BitmapFont.h
//  Date File Created: 11/23/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------

#pragma once

#include "Texture.h"
#include "Vertex.h"

namespace yae::gfx
{
struct char_desc
{
    i32 id{};
    f32 x{};
    f32 y{};
    f32 width{};
    f32 height{};
    f32 xoffset{};
    f32 yoffset{};
    f32 xadvance{};

    f32 u1{};
    f32 v1{};
    f32 u2{};
    f32 v2{};
};

class bitmap_font
{
public:
    bitmap_font(const std::string& fontname);
    bitmap_font()  = default;
    ~bitmap_font() = default;

    void load_font(const std::string& fontname);

    char_desc operator[](i32 id) const;
    char_desc operator[](char c) const { return operator[]((i32) c); }

    constexpr ID3D11ShaderResourceView* texture_view() const { return m_texture->texture_view(); }

    void build_vertex_array(std::vector<vertex_position_texture>& vertices, const std::string& text, f32 x, f32 y);

    u32 get_sentence_pixel_length(const std::string& text);

    constexpr i32 size() const { return m_size; }

    constexpr u32 space_size() const { return m_space_size; }
    void          set_space_size(u32 size) { m_space_size = size; }

private:
    texture*                           m_texture{};
    std::unordered_map<i32, char_desc> m_chars{};
    i32                                m_size{};
    u32                                m_space_size{ 30 };
};

class text_string
{
public:
    text_string(u32 max_len = 128) : m_max_length(max_len) {}
    text_string(const bitmap_font& font, const std::string& initial_text, i32 x, i32 y, u32 max_len = 128);
    ~text_string();

    bool init(const bitmap_font& font, const std::string& initial_text, i32 x, i32 y);

    void set_color(f32 r, f32 g, f32 b) { m_color = { r, g, b, 1.0f }; }

    void draw() const;
    void draw(f32 x, f32 y);
    void draw(const std::string& text);
    void draw(const std::string& text, f32 x, f32 y);

private:
    bool update_text(const std::string& text, f32 x, f32 y);

    bitmap_font                          m_font{};
    std::string                          m_text{};
    u32                                  m_max_length{ 128 };
    std::vector<vertex_position_texture> m_vertices{};
    std::vector<u32>                     m_indices{};
    math::vec4                           m_color{ 1.f, 1.f, 1.f, 1.f };
    ID3D11Buffer*                        m_vertex_buffer{};
    ID3D11Buffer*                        m_index_buffer{};
    f32                                  m_posx{ FLT_MIN };
    f32                                  m_posy{ FLT_MIN };
    f32                                  m_absolute_x{ FLT_MIN };
    f32                                  m_absolute_y{ FLT_MIN };
};


// Font library

namespace fonts
{

void init();
void unload();


bitmap_font& coolvetica();

} // namespace fonts

} // namespace yae::gfx