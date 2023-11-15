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
//  File Name: D3D11Core.cpp
//  Date File Created: 11/11/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------
#include "D3D11Core.h"


// Linking directx libraries
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;


namespace yae::gfx::core
{

namespace
{
struct video_card_info
{
    i32  memory{};
    char description[128]{};
};

video_card_info          gpu_info{};
IDXGISwapChain*          swapchain{};
ID3D11Device*            device{};
ID3D11DeviceContext*     device_context{};
ID3D11RenderTargetView*  render_target_view{};
ID3D11Texture2D*         depth_stencil_buffer{};
ID3D11DepthStencilState* depth_stencil_state{};
ID3D11DepthStencilView*  depth_stencil_view{};
ID3D11RasterizerState*   raster_state{};
math::matrix             projection_matrix{};
math::matrix             world_matrix{};
math::matrix             ortho_matrix{};
D3D11_VIEWPORT           viewport{};

bool initialized{};

DXGI_SWAP_CHAIN_DESC create_swapchain_desc(i32 width, i32 height, u32 numerator, u32 denominator, HWND hwnd)
{
    DXGI_SWAP_CHAIN_DESC swapchain_desc{};
    swapchain_desc.BufferCount       = 1;
    swapchain_desc.BufferDesc.Width  = width;
    swapchain_desc.BufferDesc.Height = height;
    swapchain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    if (g_settings->get<bool>("display", "vsync"))
    {
        swapchain_desc.BufferDesc.RefreshRate.Numerator   = numerator;
        swapchain_desc.BufferDesc.RefreshRate.Denominator = denominator;
    } else
    {
        swapchain_desc.BufferDesc.RefreshRate.Numerator   = 0;
        swapchain_desc.BufferDesc.RefreshRate.Denominator = 1;
    }

    swapchain_desc.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapchain_desc.OutputWindow = hwnd;

    // turn multisampling off, at least for now
    swapchain_desc.SampleDesc.Count   = 1;
    swapchain_desc.SampleDesc.Quality = 0;

    if (g_settings->get<bool>("display", "fullscreen"))
    {
        swapchain_desc.Windowed = false;
    } else
    {
        swapchain_desc.Windowed = true;
    }

    swapchain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapchain_desc.BufferDesc.Scaling          = DXGI_MODE_SCALING_UNSPECIFIED;

    swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    swapchain_desc.Flags = 0;

    return swapchain_desc;
}

} // anonymous namespace


bool init(i32 width, i32 height, HWND hwnd, bool fullscreen, f32 screen_depth, f32 screen_near)
{
    IDXGIFactory*     factory{};
    IDXGIAdapter*     adapter{};
    IDXGIOutput*      adapter_output{};
    u32               num_modes{};
    u32               numerator{};
    u32               denominator{};
    DXGI_ADAPTER_DESC adapter_desc{};


    DX_CALL(CreateDXGIFactory(IID_PPV_ARGS(&factory)));
    DX_CALL(factory->EnumAdapters(0, &adapter));
    DX_CALL(adapter->EnumOutputs(0, &adapter_output));
    DX_CALL(adapter_output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &num_modes, nullptr));

    std::vector<DXGI_MODE_DESC> display_mode_list{ num_modes };

    DX_CALL(adapter_output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &num_modes,
                                               display_mode_list.data()));

    for (u32 i = 0; i < num_modes; ++i)
    {
        if (display_mode_list[i].Width == (u32) width && display_mode_list[i].Height == (u32) height)
        {
            numerator   = display_mode_list[i].RefreshRate.Numerator;
            denominator = display_mode_list[i].RefreshRate.Denominator;
        }
    }

    DX_CALL(adapter->GetDesc(&adapter_desc));
    gpu_info.memory = (i32) adapter_desc.DedicatedVideoMemory / 1024 / 1024;
    size_t    string_len;
    const i32 error = wcstombs_s(&string_len, gpu_info.description, 128, adapter_desc.Description, 128);
    if (error)
    {
        return false;
    }

    release(adapter_output);
    release(adapter);
    release(factory);


    const DXGI_SWAP_CHAIN_DESC  swapchain_desc = create_swapchain_desc(width, height, numerator, denominator, hwnd);
    constexpr D3D_FEATURE_LEVEL feat_level     = D3D_FEATURE_LEVEL_11_0;

    DX_CALL(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, &feat_level, 1, D3D11_SDK_VERSION,
                                          &swapchain_desc, &swapchain, &device, nullptr, &device_context));

    ID3D11Texture2D* backbuffer_ptr;
    DX_CALL(swapchain->GetBuffer(0, IID_PPV_ARGS(&backbuffer_ptr)));

    DX_CALL(device->CreateRenderTargetView(backbuffer_ptr, nullptr, &render_target_view));

    release(backbuffer_ptr);

    D3D11_TEXTURE2D_DESC depth_buffer_desc{};
    depth_buffer_desc.Width              = width;
    depth_buffer_desc.Height             = height;
    depth_buffer_desc.MipLevels          = 1;
    depth_buffer_desc.ArraySize          = 1;
    depth_buffer_desc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depth_buffer_desc.SampleDesc.Count   = 1;
    depth_buffer_desc.SampleDesc.Quality = 0;
    depth_buffer_desc.Usage              = D3D11_USAGE_DEFAULT;
    depth_buffer_desc.BindFlags          = D3D11_BIND_DEPTH_STENCIL;
    depth_buffer_desc.CPUAccessFlags     = 0;
    depth_buffer_desc.MiscFlags          = 0;

    DX_CALL(device->CreateTexture2D(&depth_buffer_desc, nullptr, &depth_stencil_buffer));

    D3D11_DEPTH_STENCIL_DESC depth_stencil_desc{};
    depth_stencil_desc.DepthEnable      = true;
    depth_stencil_desc.DepthWriteMask   = D3D11_DEPTH_WRITE_MASK_ALL;
    depth_stencil_desc.DepthFunc        = D3D11_COMPARISON_LESS;
    depth_stencil_desc.StencilEnable    = true;
    depth_stencil_desc.StencilReadMask  = 0xff;
    depth_stencil_desc.StencilWriteMask = 0xff;

    // Stencil operations if pixel is front-facing.
    depth_stencil_desc.FrontFace.StencilFailOp      = D3D11_STENCIL_OP_KEEP;
    depth_stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depth_stencil_desc.FrontFace.StencilPassOp      = D3D11_STENCIL_OP_KEEP;
    depth_stencil_desc.FrontFace.StencilFunc        = D3D11_COMPARISON_ALWAYS;

    // Stencil operations if pixel is back-facing.
    depth_stencil_desc.BackFace.StencilFailOp      = D3D11_STENCIL_OP_KEEP;
    depth_stencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depth_stencil_desc.BackFace.StencilPassOp      = D3D11_STENCIL_OP_KEEP;
    depth_stencil_desc.BackFace.StencilFunc        = D3D11_COMPARISON_ALWAYS;

    DX_CALL(device->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_state));

    device_context->OMSetDepthStencilState(depth_stencil_state, 1);

    D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc{};
    depth_stencil_view_desc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depth_stencil_view_desc.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;
    depth_stencil_view_desc.Texture2D.MipSlice = 0;
    DX_CALL(device->CreateDepthStencilView(depth_stencil_buffer, &depth_stencil_view_desc, &depth_stencil_view));

    device_context->OMSetRenderTargets(1, &render_target_view, depth_stencil_view);

    D3D11_RASTERIZER_DESC raster_desc{};
    raster_desc.AntialiasedLineEnable = false;
    raster_desc.CullMode              = D3D11_CULL_BACK;
    raster_desc.DepthBias             = 0;
    raster_desc.DepthBiasClamp        = 0.0f;
    raster_desc.DepthClipEnable       = true;
    raster_desc.FillMode              = D3D11_FILL_SOLID;
    raster_desc.FrontCounterClockwise = false;
    raster_desc.MultisampleEnable     = false;
    raster_desc.ScissorEnable         = false;
    raster_desc.SlopeScaledDepthBias  = 0.0f;

    DX_CALL(device->CreateRasterizerState(&raster_desc, &raster_state));

    device_context->RSSetState(raster_state);

    viewport.Width    = (f32) width;
    viewport.Height   = (f32) height;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;

    device_context->RSSetViewports(1, &viewport);

    f32 fov    = math::pi / 4.f;
    f32 aspect = (f32) width / (f32) height;

    projection_matrix = XMMatrixPerspectiveFovLH(fov, aspect, screen_near, screen_depth);
    world_matrix      = XMMatrixIdentity();
    ortho_matrix      = XMMatrixOrthographicLH((f32) width, (f32) height, screen_near, screen_depth);

    initialized = true;
    return true;
}

void shutdown()
{
    if(!initialized) return;
    if (swapchain)
    {
        swapchain->SetFullscreenState(false, nullptr);
    }

    release(raster_state);
    release(depth_stencil_view);
    release(depth_stencil_state);
    release(depth_stencil_buffer);
    release(render_target_view);
    release(device_context);
    release(device);
    release(swapchain);
}

void begin_scene(f32 r, f32 g, f32 b, f32 a)
{
    const f32 color[4]{ r, g, b, a };
    device_context->ClearRenderTargetView(render_target_view, color);                     // clear back buffer
    device_context->ClearDepthStencilView(depth_stencil_view, D3D11_CLEAR_DEPTH, 1.f, 0); // clear depth buffer
}

void end_scene()
{
    if (g_settings->get<bool>("display", "vsync"))
    {
        swapchain->Present(1, 0);
    } else
    {
        swapchain->Present(0, 0);
    }
}

ID3D11Device* get_device()
{
    return device;
}
ID3D11DeviceContext* get_device_context()
{
    return device_context;
}
math::matrix get_projection_matrix()
{
    return projection_matrix;
}
math::matrix get_world_matrix()
{
    return world_matrix;
}
math::matrix get_orthographic_matrix()
{
    return ortho_matrix;
}

void get_video_card_info(char* card_name, i32& memory)
{
    strcpy_s(card_name, 128, gpu_info.description);
    memory = gpu_info.memory;
}

void set_back_buffer_render_target()
{
    device_context->OMSetRenderTargets(1, &render_target_view, depth_stencil_view);
}

void reset_viewport()
{
    device_context->RSSetViewports(1, &viewport);
}

} // namespace yae::gfx::core