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
//  File Name: Geometry.cpp
//  Date File Created: 11/20/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------
#include "Geometry.h"


#include "Yae/Core/System.h"

namespace yae::gfx::geometry
{

void create_plane(std::vector<vertex_position_normal_texture>& vertices, std::vector<u32>& indices, u32 width, u32 height)
{
    vertices.resize(4);
    indices.resize(6);

    // clang-format off
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
}

void create_quad(std::vector<vertex_position_texture>& vertices, std::vector<u32>& indices, u32 width, u32 height)
{
    vertices.resize(4);
    indices.resize(6);

    // clang-format off
    vertices[0] = { { -1.0f * (f32)width * 0.5f, -1.0f * (f32)height * 0.5f, 0.0f }, { 0.0f, 1.0f } };
	vertices[1] = { { -1.0f * (f32)width * 0.5f,  1.0f * (f32)height * 0.5f, 0.0f }, { 0.0f, 0.0f } };
	vertices[2] = { {  1.0f * (f32)width * 0.5f,  1.0f * (f32)height * 0.5f, 0.0f }, { 1.0f, 0.0f } };
	vertices[3] = { {  1.0f * (f32)width * 0.5f, -1.0f * (f32)height * 0.5f, 0.0f }, { 1.0f, 1.0f } };
    // clang-format on


    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;

    indices[3] = 0;
    indices[4] = 2;
    indices[5] = 3;
}

void create_sphere(std::vector<vertex_position_normal_texture>& vertices, std::vector<u32>& indices, f32 radius, u32 slice_count,
                   u32 stack_count)
{
    vertices.clear();
    indices.clear();

    // clang-format off
	//Vertices
	const vertex_position_normal_texture top_vertex   { { 0.0f,  radius, 0.0f }, { 0.0f,  1.0f, 0.0f }, { 0.0f, 0.0f } };
	const vertex_position_normal_texture bottom_vertex{ { 0.0f, -radius, 0.0f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 1.0f } };
    // clang-format on


    vertices.push_back(top_vertex);

    const f32 phi_step   = math::pi / (f32) stack_count;
    const f32 theta_step = 2.0f * math::pi / (f32) slice_count;

    for (u32 stack = 1; stack < stack_count; stack++)
    {
        f32 phi = (f32) stack * phi_step;

        for (u32 slice = 0; slice < slice_count + 1; slice++)
        {
            f32 theta = (f32) slice * theta_step;

            vertex_position_normal_texture vertex;
            vertex.position = { radius * sinf(phi) * cosf(theta), radius * cosf(phi), radius * sinf(phi) * sinf(theta) };

            vertex.texture = { theta / XM_2PI, phi / (f32) math::pi };

            XMStoreFloat3(&vertex.normal, XMVector3Normalize(XMLoadFloat3(&vertex.position)));

            vertices.push_back(vertex);
        }
    }
    vertices.push_back(bottom_vertex);


    // Top
    for (u32 slice = 0; slice < slice_count + 1; slice++)
    {
        indices.push_back(0);
        indices.push_back(slice + 1);
        indices.push_back(slice);
    }

    // Middle
    u32       base_idx       = 1;
    const u32 ring_vtx_count = slice_count + 1;
    for (u32 stack = 0; stack < stack_count - 2; stack++)
    {
        for (u32 slice = 0; slice < slice_count; slice++)
        {
            indices.push_back(base_idx + stack * ring_vtx_count + slice);
            indices.push_back(base_idx + stack * ring_vtx_count + slice + 1);
            indices.push_back(base_idx + (stack + 1) * ring_vtx_count + slice);

            indices.push_back(base_idx + (stack + 1) * ring_vtx_count + slice);
            indices.push_back(base_idx + stack * ring_vtx_count + slice + 1);
            indices.push_back(base_idx + (stack + 1) * ring_vtx_count + slice + 1);
        }
    }

    // Bottom
    const u32 south_pole_idx = (u32) vertices.size() - 1;
    base_idx                 = south_pole_idx - ring_vtx_count;

    for (u32 slice = 0; slice < slice_count; slice++)
    {
        indices.push_back(south_pole_idx);
        indices.push_back(base_idx + slice);
        indices.push_back(base_idx + slice + 1);
    }
}

void create_box(std::vector<vertex_position_normal_texture>& vertices, std::vector<u32>& indices, f32 width, f32 height,
                f32 depth)
{
    vertices.clear();
    indices.clear();

    vertex_position_normal_texture v[24];

    float w2 = 0.5f * width;
    float h2 = 0.5f * height;
    float d2 = 0.5f * depth;

    // clang-format off

	// Fill in the front face vertex data.
	v[0]  = { { -w2, -h2, -d2 }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f } };
	v[1]  = { { -w2, +h2, -d2 }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f } };
	v[2]  = { { +w2, +h2, -d2 }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f } };
	v[3]  = { { +w2, -h2, -d2 }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f } };

	// Fill in the back face vertex data.
	v[4]  = { { -w2, -h2, +d2 }, { 0.0f, 0.0f,  1.0f }, { 1.0f, 1.0f } };
	v[5]  = { { +w2, -h2, +d2 }, { 0.0f, 0.0f,  1.0f }, { 0.0f, 1.0f } };
	v[6]  = { { +w2, +h2, +d2 }, { 0.0f, 0.0f,  1.0f }, { 0.0f, 0.0f } };
	v[7]  = { { -w2, +h2, +d2 }, { 0.0f, 0.0f,  1.0f }, { 1.0f, 0.0f } };

	// Fill in the top face vertex data.
	v[8]  = { { -w2, +h2, -d2 }, { 0.0f,  1.0f, 0.0f }, { 0.0f, 1.0f } };
	v[9]  = { { -w2, +h2, +d2 }, { 0.0f,  1.0f, 0.0f }, { 0.0f, 0.0f } };
	v[10] = { { +w2, +h2, +d2 }, { 0.0f,  1.0f, 0.0f }, { 1.0f, 0.0f } };
	v[11] = { { +w2, +h2, -d2 }, { 0.0f,  1.0f, 0.0f }, { 1.0f, 1.0f } };

	// Fill in the bottom face vertex data.
	v[12] = { { -w2, -h2, -d2 }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 1.0f } };
	v[13] = { { +w2, -h2, -d2 }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 1.0f } };
	v[14] = { { +w2, -h2, +d2 }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f } };
	v[15] = { { -w2, -h2, +d2 }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 0.0f } };

	// Fill in the left face vertex data.
	v[16] = { { -w2, -h2, +d2 }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } };
	v[17] = { { -w2, +h2, +d2 }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } };
	v[18] = { { -w2, +h2, -d2 }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } };
	v[19] = { { -w2, -h2, -d2 }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } };

	// Fill in the right face vertex data.
	v[20] = { { +w2, -h2, -d2 }, {  1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } };
	v[21] = { { +w2, +h2, -d2 }, {  1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } };
	v[22] = { { +w2, +h2, +d2 }, {  1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } };
	v[23] = { { +w2, -h2, +d2 }, {  1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } };

	vertices.assign(&v[0], &v[24]);

	UINT i[36];

	// Fill in the front face index data
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;

	// Fill in the back face index data
	i[6] = 4; i[7] = 5; i[8] = 6;
	i[9] = 4; i[10] = 6; i[11] = 7;

	// Fill in the top face index data
	i[12] = 8; i[13] = 9; i[14] = 10;
	i[15] = 8; i[16] = 10; i[17] = 11;

	// Fill in the bottom face index data
	i[18] = 12; i[19] = 13; i[20] = 14;
	i[21] = 12; i[22] = 14; i[23] = 15;

	// Fill in the left face index data
	i[24] = 16; i[25] = 17; i[26] = 18;
	i[27] = 16; i[28] = 18; i[29] = 19;

	// Fill in the right face index data
	i[30] = 20; i[31] = 21; i[32] = 22;
	i[33] = 20; i[34] = 22; i[35] = 23;

    // clang-format on

    indices.assign(&i[0], &i[36]);
}
model* create_sphere(f32 radius, u32 slice_count, u32 stack_count)
{
    auto* ret = DBG_NEW                         model{};
    std::vector<vertex_position_normal_texture> verts{};
    std::vector<u32>                            ints{};
    create_sphere(verts, ints, radius, slice_count, stack_count);
    ret->init(verts, ints);

    return ret;
}

model* create_box(f32 width, f32 height, f32 depth)
{
    auto* ret = DBG_NEW                         model{};
    std::vector<vertex_position_normal_texture> verts{};
    std::vector<u32>                            ints{};
    create_box(verts, ints, width, height, depth);
    ret->init(verts, ints);

    return ret;
}

model* create_plane(u32 width, u32 height)
{
    auto* ret = DBG_NEW                         model{};
    std::vector<vertex_position_normal_texture> verts{};
    std::vector<u32>                            ints{};
    create_plane(verts, ints, width, height);
    ret->init(verts, ints);
    return ret;
}

model* create_quad(u32 width, u32 height)
{
    auto*                                ret = new model{};
    std::vector<vertex_position_texture> verts{};
    std::vector<u32>                     ints{};
    create_quad(verts, ints, width, height);
    ret->init(verts, ints);
    return ret;
}

} // namespace yae::gfx::geometry