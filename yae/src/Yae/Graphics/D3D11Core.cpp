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


#include "Renderer.h"
#include "Shaders/ShaderLibrary.h"
#include "Yae/Core/System.h"

// Linking directx libraries
//#pragma comment(lib, "d3d11.lib")
//#pragma comment(lib, "dxgi.lib")
//#pragma comment(lib, "d3dcompiler.lib")

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


// Deferred rendering stuff
constexpr f32 clear_color[4]  = { 0.f, 0.f, 0.f, 1.f };
constexpr f32 blend_factor[4] = { 1.f, 1.f, 1.f, 1.f };
constexpr u32 buffer_count    = 3;

ID3D11RenderTargetView*   rtv_array[buffer_count];
ID3D11ShaderResourceView* srv_array[buffer_count];

ID3D11Texture2D*         dr_depth_stencil_buffer{};
ID3D11DepthStencilState* dr_depth_stencil_state{};
ID3D11DepthStencilView*  dr_depth_stencil_view{};
ID3D11BlendState*        dr_blend_state{};
ID3D11RasterizerState*   dr_raster_state{};
//D3D11_VIEWPORT           dr_viewport{};

video_card_info          gpu_info{};
IDXGISwapChain*          swapchain{};
ID3D11Device*            device{};
ID3D11DeviceContext*     device_context{};
ID3D11RenderTargetView*  render_target_view{};
ID3D11Texture2D*         depth_stencil_buffer{};
ID3D11DepthStencilState* depth_stencil_state{};
ID3D11DepthStencilState* disabled_depth_stencil_state{};
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


bool init_deferred_renderer(ID3D11Device* device)
{
    // Texture for Position and Normal are 32 bit
    D3D11_TEXTURE2D_DESC pos_norm_desc{};
    pos_norm_desc.Width              = system::width();
    pos_norm_desc.Height             = system::height();
    pos_norm_desc.MipLevels          = 1;
    pos_norm_desc.ArraySize          = 1;
    pos_norm_desc.Format             = DXGI_FORMAT_R32G32B32A32_FLOAT;
    pos_norm_desc.SampleDesc.Count   = 1;
    pos_norm_desc.SampleDesc.Quality = 0;
    pos_norm_desc.Usage              = D3D11_USAGE_DEFAULT;
    pos_norm_desc.BindFlags          = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    pos_norm_desc.CPUAccessFlags     = 0;
    pos_norm_desc.MiscFlags          = 0;

    // Texture for diffuse is 8 bit
    D3D11_TEXTURE2D_DESC diffuse_desc{};
    diffuse_desc.Width              = system::width();
    diffuse_desc.Height             = system::height();
    diffuse_desc.MipLevels          = 1;
    diffuse_desc.ArraySize          = 1;
    diffuse_desc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
    diffuse_desc.SampleDesc.Count   = 1;
    diffuse_desc.SampleDesc.Quality = 0;
    diffuse_desc.Usage              = D3D11_USAGE_DEFAULT;
    diffuse_desc.BindFlags          = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    diffuse_desc.CPUAccessFlags     = 0;
    diffuse_desc.MiscFlags          = 0;

    ID3D11Texture2D* rtvs[buffer_count];
    DX_CALL(device->CreateTexture2D(&pos_norm_desc, nullptr, &rtvs[0]));
    DX_CALL(device->CreateTexture2D(&pos_norm_desc, nullptr, &rtvs[1]));
    DX_CALL(device->CreateTexture2D(&pos_norm_desc, nullptr, &rtvs[2]));

    D3D11_RENDER_TARGET_VIEW_DESC rtv_desc32{};
    rtv_desc32.Format             = pos_norm_desc.Format;
    rtv_desc32.ViewDimension      = D3D11_RTV_DIMENSION_TEXTURE2D;
    rtv_desc32.Texture2D.MipSlice = 0;

    D3D11_RENDER_TARGET_VIEW_DESC rtv_desc8{};
    rtv_desc8.Format             = pos_norm_desc.Format;
    rtv_desc8.ViewDimension      = D3D11_RTV_DIMENSION_TEXTURE2D;
    rtv_desc8.Texture2D.MipSlice = 0;

    DX_CALL(device->CreateRenderTargetView(rtvs[0], &rtv_desc32, &rtv_array[0]));
    DX_CALL(device->CreateRenderTargetView(rtvs[1], &rtv_desc32, &rtv_array[1]));
    DX_CALL(device->CreateRenderTargetView(rtvs[2], &rtv_desc8, &rtv_array[2]));

    D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc{};
    srv_desc.Format                    = pos_norm_desc.Format;
    srv_desc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
    srv_desc.Texture2D.MostDetailedMip = 0;
    srv_desc.Texture2D.MipLevels       = 1;

    for (u32 i = 0; i < buffer_count; ++i)
    {
        DX_CALL(device->CreateShaderResourceView(rtvs[i], &srv_desc, &srv_array[i]));
    }

    for (u32 i = 0; i < buffer_count; ++i)
    {
        release(rtvs[i]);
    }

    D3D11_TEXTURE2D_DESC depth_desc{};
    depth_desc.Width              = system::width();
    depth_desc.Height             = system::height();
    depth_desc.MipLevels          = 1;
    depth_desc.ArraySize          = 1;
    depth_desc.Format             = DXGI_FORMAT_R24G8_TYPELESS;
    depth_desc.SampleDesc.Count   = 1;
    depth_desc.SampleDesc.Quality = 0;
    depth_desc.Usage              = D3D11_USAGE_DEFAULT;
    depth_desc.BindFlags          = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    depth_desc.CPUAccessFlags     = 0;
    depth_desc.MiscFlags          = 0;

    DX_CALL(device->CreateTexture2D(&depth_desc, nullptr, &dr_depth_stencil_buffer));

    D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc{};
    dsv_desc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsv_desc.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsv_desc.Texture2D.MipSlice = 0;

    DX_CALL(device->CreateDepthStencilView(dr_depth_stencil_buffer, &dsv_desc, &dr_depth_stencil_view));

    D3D11_RASTERIZER_DESC rast_desc{};
    rast_desc.CullMode        = D3D11_CULL_BACK;
    rast_desc.FillMode        = D3D11_FILL_SOLID;
    rast_desc.DepthClipEnable = false;

    DX_CALL(device->CreateRasterizerState(&rast_desc, &dr_raster_state));

    D3D11_BLEND_DESC blend_desc{};
    blend_desc.AlphaToCoverageEnable                 = false;
    blend_desc.IndependentBlendEnable                = false;
    blend_desc.RenderTarget[0].BlendEnable           = true;
    blend_desc.RenderTarget[0].SrcBlend              = D3D11_BLEND_ONE;
    blend_desc.RenderTarget[0].DestBlend             = D3D11_BLEND_ONE;
    blend_desc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;
    blend_desc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;
    blend_desc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ONE;
    blend_desc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
    blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    DX_CALL(device->CreateBlendState(&blend_desc, &dr_blend_state));

    D3D11_DEPTH_STENCIL_DESC ds_desc{};
    ds_desc.DepthEnable    = true;
    ds_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    ds_desc.DepthFunc      = D3D11_COMPARISON_LESS_EQUAL;

    DX_CALL(device->CreateDepthStencilState(&ds_desc, &dr_depth_stencil_state));

    return true;
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

    D3D11_DEPTH_STENCIL_DESC disabled_desc{};
    disabled_desc.DepthEnable                  = false;
    disabled_desc.DepthWriteMask               = D3D11_DEPTH_WRITE_MASK_ALL;
    disabled_desc.DepthFunc                    = D3D11_COMPARISON_LESS;
    disabled_desc.StencilEnable                = true;
    disabled_desc.StencilReadMask              = 0xFF;
    disabled_desc.StencilWriteMask             = 0xFF;
    disabled_desc.FrontFace.StencilFailOp      = D3D11_STENCIL_OP_KEEP;
    disabled_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    disabled_desc.FrontFace.StencilPassOp      = D3D11_STENCIL_OP_KEEP;
    disabled_desc.FrontFace.StencilFunc        = D3D11_COMPARISON_ALWAYS;
    disabled_desc.BackFace.StencilFailOp       = D3D11_STENCIL_OP_KEEP;
    disabled_desc.BackFace.StencilDepthFailOp  = D3D11_STENCIL_OP_DECR;
    disabled_desc.BackFace.StencilPassOp       = D3D11_STENCIL_OP_KEEP;
    disabled_desc.BackFace.StencilFunc         = D3D11_COMPARISON_ALWAYS;

    DX_CALL(device->CreateDepthStencilState(&disabled_desc, &disabled_depth_stencil_state));

    if (!init_deferred_renderer(device))
    {
        LOG_FATAL("Failed to initialize deferred renderer");
        return false;
    }

    if (!shaders::init())
    {
        LOG_ERROR("Failed to initialize engine shaders");
        return false;
    }

    init_renderer();

    initialized = true;
    return true;
}

void shutdown()
{
    if (!initialized)
        return;

    shaders::shutdown();

    if (swapchain)
    {
        swapchain->SetFullscreenState(false, nullptr);
    }

    shutdown_renderer();
    release(dr_depth_stencil_buffer);
    release(dr_depth_stencil_view);
    release(dr_raster_state);
    release(dr_blend_state);
    release(dr_depth_stencil_state);

    for (u32 i = 0; i < buffer_count; ++i)
    {
        release(rtv_array[i]);
        release(srv_array[i]);
    }

    release(disabled_depth_stencil_state);
    release(raster_state);
    release(depth_stencil_view);
    release(depth_stencil_state);
    release(depth_stencil_buffer);
    release(render_target_view);
    release(device_context);
    release(device);
    release(swapchain);
}

void clear_first_stage()
{
    ID3D11ShaderResourceView* null[] = { nullptr, nullptr, nullptr };
    device_context->PSSetShaderResources(0, 3, null);
    device_context->OMSetRenderTargets(3, rtv_array, dr_depth_stencil_view);
    device_context->RSSetViewports(1, &viewport);
    device_context->ClearRenderTargetView(rtv_array[0], clear_color);
    device_context->ClearRenderTargetView(rtv_array[1], clear_color);
    device_context->ClearRenderTargetView(rtv_array[2], clear_color);
    device_context->ClearDepthStencilView(dr_depth_stencil_view, D3D11_CLEAR_DEPTH, 1.f, 0);
}

void clear_second_stage()
{
    device_context->OMSetRenderTargets(1, &render_target_view, dr_depth_stencil_view);
    device_context->RSSetViewports(1, &viewport);
    device_context->ClearRenderTargetView(render_target_view, clear_color);
    device_context->ClearDepthStencilView(dr_depth_stencil_view, D3D11_CLEAR_DEPTH, 1.f, 0);
    device_context->RSSetState(dr_raster_state);

    device_context->OMSetBlendState(dr_blend_state, blend_factor, 0xFFFFFFFF);
    device_context->OMSetDepthStencilState(dr_depth_stencil_state, 0);
}

void begin_scene(f32 r, f32 g, f32 b, f32 a)
{
    const f32 color[4]{ r, g, b, a };
    device_context->ClearRenderTargetView(render_target_view, color);                     // clear back buffer
    device_context->ClearDepthStencilView(depth_stencil_view, D3D11_CLEAR_DEPTH, 1.f, 0); // clear depth buffer
}

void end_scene()
{
    device_context->RSSetState(nullptr);
    device_context->OMSetBlendState(nullptr, blend_factor, 0xFFFFFFFF);
    device_context->OMSetDepthStencilState(nullptr, 0);
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

ID3D11ShaderResourceView* position_gbuffer()
{
    return srv_array[0];
}

ID3D11ShaderResourceView* normal_gbuffer()
{
    return srv_array[1];
}
ID3D11ShaderResourceView* diffuse_gbuffer()
{
    return srv_array[2];
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

void set_world_matrix(const math::matrix& mat)
{
    world_matrix = mat;
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

void enable_zbuffer()
{
    device_context->OMSetDepthStencilState(depth_stencil_state, 1);
}

void disable_zbuffer()
{
    device_context->RSSetState(nullptr);
    device_context->OMSetBlendState(nullptr, blend_factor, 0xFFFFFFFF);
    //device_context->OMSetDepthStencilState(nullptr, 0);
    device_context->OMSetDepthStencilState(disabled_depth_stencil_state, 1);
}


} // namespace yae::gfx::core