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

#define DX_CALL(x)                                                                                                               \
    if (FAILED((x)))                                                                                                             \
    return false

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
        if (display_mode_list[i].Width == (u32)width && display_mode_list[i].Height == (u32)height)
        {
            numerator   = display_mode_list[i].RefreshRate.Numerator;
            denominator = display_mode_list[i].RefreshRate.Denominator;
        }
    }
}

void                 shutdown() {}
void                 begin_scene(f32 r, f32 g, f32 b, f32 a) {}
void                 end_scene() {}
ID3D11Device*        get_device() {}
ID3D11DeviceContext* get_device_context() {}
math::matrix         get_projection_matrix() {}
math::matrix         get_world_matrix() {}
math::matrix         get_orthographic_matrix() {}
void                 get_video_card_info(char* card_name, i32& memory) {}
void                 set_back_buffer_render_target() {}
void                 reset_viewport() {}
} // namespace yae::gfx::core