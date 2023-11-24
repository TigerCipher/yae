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
//  File Name: BitmapFont.cpp
//  Date File Created: 11/23/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------
#include "BitmapFont.h"


#include "D3D11Core.h"
#include "Renderer.h"
#include "Shaders/Shader.h"
#include "Shaders/ShaderLibrary.h"
#include "Yae/Core/System.h"


#include <ranges>

namespace yae::gfx
{

bitmap_font::bitmap_font(const std::string& fontname)
{
    load_font(fontname);
}

void bitmap_font::load_font(const std::string& fontname)
{
    if(!m_texture.init(std::format("{}.tga", fontname).c_str()))
    {
        LOG_ERROR("Failed to load texture file for font {}", fontname);
    }
    std::ifstream fin{ std::format("{}.fnt", fontname) };

    std::string line;
    while (std::getline(fin, line))
    {
        if (line.find("info face") == 0)
        {
            std::istringstream iss{ line.substr(9) };
            while (iss >> std::ws)
            {
                std::string attribute;
                if (!(iss >> attribute))
                {
                    break;
                }
                size_t equalpos = attribute.find('=');
                if (equalpos == std::string::npos)
                {
                    continue;
                }

                std::string attrib_name  = attribute.substr(0, equalpos);
                std::string attrib_value = attribute.substr(equalpos + 1);

                if (attrib_name == "size")
                {
                    m_size = std::stoi(attrib_value);
                    break;
                }
            }
        }
        if (line.find("char ") == 0)
        {
            char_desc          character{};
            std::istringstream iss{ line.substr(5) };

            while (iss >> std::ws)
            {
                std::string attribute;
                if (!(iss >> attribute))
                {
                    break; // No more attributes
                }

                size_t equalpos = attribute.find('=');
                if (equalpos == std::string::npos)
                {
                    continue; // Invalid attribute format
                }

                std::string attrib_name  = attribute.substr(0, equalpos);
                std::string attrib_value = attribute.substr(equalpos + 1);

                if (attrib_name == "id")
                {
                    character.id = std::stoi(attrib_value);
                } else if (attrib_name == "x")
                {
                    character.x = std::stof(attrib_value);
                } else if (attrib_name == "y")
                {
                    character.y = std::stof(attrib_value);
                } else if (attrib_name == "width")
                {
                    character.width = std::stof(attrib_value);
                } else if (attrib_name == "height")
                {
                    character.height = std::stof(attrib_value);
                } else if (attrib_name == "xoffset")
                {
                    character.xoffset = std::stof(attrib_value);
                } else if (attrib_name == "yoffset")
                {
                    character.yoffset = std::stof(attrib_value);
                } else if (attrib_name == "xadvance")
                {
                    character.xadvance = std::stof(attrib_value);
                }
                // Add more conditions for other attributes if needed
            }

            character.u1 = character.x / m_texture.width();
            character.v1 = character.y / m_texture.height();
            character.u2 = (character.x + character.width) / m_texture.width();
            character.v2 = (character.y + character.height) / m_texture.height();

            m_chars[character.id] = character;
        }
    }

    for (auto val : m_chars | std::views::values)
    {
        char_desc c = val;
        LOG_DEBUG("Character id={} (char: {}), x={}, y={}, u1={}, v1={}, u2={}, v2={}", c.id, (char) c.id, c.x, c.y, c.u1, c.v1,
                  c.u2, c.v2);
    }
}

char_desc bitmap_font::operator[](i32 id) const
{
    const auto it = m_chars.find(id);
    if (it == m_chars.end())
    {
        LOG_WARN("Character '{}' (id: {}) not included in this font", (char) id, id);
        return {};
    }

    return it->second;
}

void bitmap_font::build_vertex_array(std::vector<vertex_position_texture>& vertices, const std::string& text, f32 x, f32 y)
{
    const u32 num_letters = text.size();
    u32       index{};
    i32       letter{};

    /*
     *
     *    // clang-format off
    vertices[0] = { { -1.0f * (f32)width * 0.5f, -1.0f * (f32)height * 0.5f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f } };
	vertices[1] = { { -1.0f * (f32)width * 0.5f,  1.0f * (f32)height * 0.5f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f } };
	vertices[2] = { {  1.0f * (f32)width * 0.5f,  1.0f * (f32)height * 0.5f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f } };
	vertices[3] = { {  1.0f * (f32)width * 0.5f, -1.0f * (f32)height * 0.5f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f } };
    // clang-format on


    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;

    indices[3] = 0;
    indices[4] = 2;
    indices[5] = 3;
     *
     */

    for (u32 i = 0; i < num_letters; ++i)
    {
        letter = (i32) text[i];
        if (letter == (i32) ' ')
        {
            x = x + m_space_size;
            continue;
        }

        f32 yoffset = m_chars[letter].yoffset;
        f32 xoffset = m_chars[letter].xoffset;
        vertices[index].position = { x, y - yoffset, 0.f };
        vertices[index].texture  = { m_chars[letter].u1, m_chars[letter].v1 };
        ++index;

        vertices[index].position = { x + m_chars[letter].width, y - m_chars[letter].height - yoffset, 0.f };
        vertices[index].texture  = { m_chars[letter].u2, m_chars[letter].v2 };
        ++index;

        vertices[index].position = { x, y - m_chars[letter].height - yoffset, 0.f };
        vertices[index].texture  = { m_chars[letter].u1, m_chars[letter].v2 };
        ++index;

        vertices[index].position = { x, y - yoffset, 0.f };
        vertices[index].texture  = { m_chars[letter].u1, m_chars[letter].v1 };
        ++index;

        vertices[index].position = { x + m_chars[letter].width, y - yoffset, 0.f };
        vertices[index].texture  = { m_chars[letter].u2, m_chars[letter].v1 };
        ++index;

        vertices[index].position = { x + m_chars[letter].width, y - m_chars[letter].height - yoffset, 0.f };
        vertices[index].texture  = { m_chars[letter].u2, m_chars[letter].v2 };
        ++index;

        x = x + m_chars[letter].width + xoffset; // + 1.f
    }
}

u32 bitmap_font::get_sentence_pixel_length(const std::string& text)
{
    u32 pixel_len{};

    const u32 num_letters = text.size();

    for (u32 i = 0; i < num_letters; ++i)
    {
        i32 letter = (i32) text[i];
        if (letter == (i32) ' ')
        {
            pixel_len += m_space_size;
            continue;
        }

        pixel_len += m_chars[letter].width + 1;
    }

    return pixel_len;
}


text_string::text_string(const bitmap_font& font, const std::string& initial_text, i32 x, i32 y, u32 max_len) :
    m_max_length(max_len)
{
    init(font, initial_text, x, y);
}

text_string::~text_string()
{
    core::release(m_vertex_buffer);
    core::release(m_index_buffer);
}

bool text_string::init(const bitmap_font& font, const std::string& initial_text, i32 x, i32 y)
{
    D3D11_BUFFER_DESC      vtx_desc{};
    D3D11_BUFFER_DESC      idx_desc{};
    D3D11_SUBRESOURCE_DATA vtx_data{};
    D3D11_SUBRESOURCE_DATA idx_data{};

    m_vertices.resize(6 * m_max_length);
    m_indices.resize(6 * m_max_length);

    for (u32 i = 0; i < m_indices.size(); ++i)
    {
        m_indices[i] = i;
    }

    vtx_desc.Usage               = D3D11_USAGE_DYNAMIC;
    vtx_desc.ByteWidth           = sizeof(vertex_position_texture) * m_vertices.size();
    vtx_desc.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
    vtx_desc.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
    vtx_desc.MiscFlags           = 0;
    vtx_desc.StructureByteStride = 0;

    vtx_data.pSysMem          = m_vertices.data();
    vtx_data.SysMemPitch      = 0;
    vtx_data.SysMemSlicePitch = 0;

    DX_CALL(core::get_device()->CreateBuffer(&vtx_desc, &vtx_data, &m_vertex_buffer));

    idx_desc.Usage               = D3D11_USAGE_DEFAULT;
    idx_desc.ByteWidth           = sizeof(u32) * m_indices.size();
    idx_desc.BindFlags           = D3D11_BIND_INDEX_BUFFER;
    idx_desc.CPUAccessFlags      = 0;
    idx_desc.MiscFlags           = 0;
    idx_desc.StructureByteStride = 0;

    idx_data.pSysMem          = m_indices.data();
    idx_data.SysMemPitch      = 0;
    idx_data.SysMemSlicePitch = 0;

    DX_CALL(core::get_device()->CreateBuffer(&idx_desc, &idx_data, &m_index_buffer));

    m_font = font;
    return update_text(initial_text, x, y);
}

void text_string::draw() const
{
    vertex_shader* vs = shaders::font()->vs;
    pixel_shader*  ps = shaders::font()->ps;

    vs->set_matrix("worldMatrix", XMMatrixTranspose(XMMatrixIdentity()));
    vs->set_matrix("viewMatrix", XMMatrixTranspose(default_view_matrix()));
    vs->set_matrix("projectionMatrix", XMMatrixTranspose(core::get_orthographic_matrix()));

    vs->copy_all_buffers();
    vs->bind();


    ps->set_shader_resource_view("diffuse", m_font.texture_view());
    ps->set_sampler_state("Sampler", default_sampler_state());
    ps->set_float4("fontColor", m_color);

    ps->copy_all_buffers();
    ps->bind();

    constexpr u32 stride = sizeof(vertex_position_texture);
    constexpr u32 offset{};

    core::get_device_context()->IASetVertexBuffers(0, 1, &m_vertex_buffer, &stride, &offset);
    core::get_device_context()->IASetIndexBuffer(m_index_buffer, DXGI_FORMAT_R32_UINT, 0);
    core::get_device_context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    core::get_device_context()->DrawIndexed(m_indices.size(), 0, 0);
}

void text_string::draw(f32 x, f32 y)
{
    update_text(m_text, x, y);
    draw();
}

void text_string::draw(const std::string& text)
{
    update_text(text, m_absolute_x, m_absolute_y);
    draw();
}

void text_string::draw(const std::string& text, f32 x, f32 y)
{
    update_text(text, x, y);
    draw();
}

bool text_string::update_text(const std::string& text, f32 x, f32 y)
{
    if (m_text == text && m_absolute_x == x && m_absolute_y == y)
    {
        return false;
    }
    D3D11_MAPPED_SUBRESOURCE mapped_res{};

    m_vertices.clear();
    m_vertices.resize(6 * m_max_length);
    m_text       = text;
    m_absolute_x = x;
    m_absolute_y = y;
    m_posx       = -(f32) system::width() / 2.f + x;
    m_posy       = (f32) system::height() / 2.f - y;

    m_font.build_vertex_array(m_vertices, text, m_posx, m_posy);

    DX_CALL(core::get_device_context()->Map(m_vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_res));
    auto* ptr = (vertex_position_texture*) mapped_res.pData;
    CopyMemory(ptr, m_vertices.data(), sizeof(vertex_position_texture) * m_vertices.size());
    core::get_device_context()->Unmap(m_vertex_buffer, 0);

    return true;
}

} // namespace yae::gfx