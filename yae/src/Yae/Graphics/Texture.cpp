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
//  File Name: Texture.cpp
//  Date File Created: 11/13/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------
#include "Texture.h"


#include "D3D11Core.h"
#include "Yae/Core/System.h"

namespace yae::gfx
{

texture::texture(const char* filename)
{
    init(filename);
}

bool texture::init(const char* filename)
{
    if (!load_targa_32bit(filename))
    {
        return false;
    }

    D3D11_TEXTURE2D_DESC desc{};
    desc.Height             = m_height;
    desc.Width              = m_width;
    desc.MipLevels          = 0;
    desc.ArraySize          = 1;
    desc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count   = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage              = D3D11_USAGE_DEFAULT;
    desc.BindFlags          = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    desc.CPUAccessFlags     = 0;
    desc.MiscFlags          = D3D11_RESOURCE_MISC_GENERATE_MIPS;

    DX_CALL(core::get_device()->CreateTexture2D(&desc, nullptr, &m_texture));

    u32 row_pitch = m_width * 4 * (u32) sizeof(u8);

    core::get_device_context()->UpdateSubresource(m_texture, 0, nullptr, m_data.data(), row_pitch, 0);

    D3D11_SHADER_RESOURCE_VIEW_DESC srv{};
    srv.Format                    = desc.Format;
    srv.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
    srv.Texture2D.MostDetailedMip = 0;
    srv.Texture2D.MipLevels       = -1;

    DX_CALL(core::get_device()->CreateShaderResourceView(m_texture, &srv, &m_texture_view));

    core::get_device_context()->GenerateMips(m_texture_view);

    // Sampler state
    D3D11_SAMPLER_DESC sampler_desc;
    sampler_desc.Filter         = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampler_desc.AddressU       = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc.AddressV       = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc.AddressW       = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc.MipLODBias     = 0.f;
    sampler_desc.MaxAnisotropy  = 1;
    sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    sampler_desc.BorderColor[0] = 0.f;
    sampler_desc.BorderColor[1] = 0.f;
    sampler_desc.BorderColor[2] = 0.f;
    sampler_desc.BorderColor[3] = 0.f;
    sampler_desc.MinLOD         = 0.f;
    sampler_desc.MaxLOD         = D3D11_FLOAT32_MAX;

    DX_CALL(core::get_device()->CreateSamplerState(&sampler_desc, &m_sampler_state));

    return true;
}

void texture::shutdown()
{
    core::release(m_texture_view);
    core::release(m_texture);
    core::release(m_sampler_state);
}

bool texture::load_targa_32bit(const char* filename)
{
    LOG_INFO("Loading texture from {}", filename);
    FILE* fptr;
    i32   error = fopen_s(&fptr, filename, "rb");
    if (error)
    {
        return false;
    }

    targa_header header;
    u32          count = (u32) fread(&header, sizeof(targa_header), 1, fptr);
    if (count != 1)
    {
        return false;
    }

    m_width  = (u32) header.width;
    m_height = (u32) header.height;
    i32 bpp  = header.bpp;

    if (bpp != 32)
    {
        popup::show("Texture file must be 32bit", "Unsupported bit format", popup::style::error);
        return false;
    }

    const i32 image_size = m_width * m_height * 4;

    std::vector<u8> image(image_size);

    count = (u32) fread(image.data(), 1, image_size, fptr);
    if (count != image_size)
    {
        return false;
    }

    error = fclose(fptr);
    if (error)
    {
        return false;
    }

    m_data.resize(image_size);

    i32 index = 0;
    i32 k     = (m_width * m_height * 4) - (m_width * 4);
    for (u32 j = 0; j < m_height; ++j)
    {
        for (u32 i = 0; i < m_width; ++i)
        {
            m_data[index + 0] = image[k + 2]; // red
            m_data[index + 1] = image[k + 1]; // green
            m_data[index + 2] = image[k + 0]; // blue
            m_data[index + 3] = image[k + 3]; // alpha

            k += 4;
            index += 4;
        }

        k -= m_width * 8;
    }

    LOG_INFO("Done.");

    return true;
}

} // namespace yae::gfx