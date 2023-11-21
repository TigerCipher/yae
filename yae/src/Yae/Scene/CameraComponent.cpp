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
//  File Name: CameraComponent.cpp
//  Date File Created: 11/20/2023
//  Author: Matt
//
//  ------------------------------------------------------------------------------
#include "CameraComponent.h"

#include "Yae/Core/Application.h"

namespace yae
{
camera_component::camera_component()
{
    m_view = XMMatrixIdentity();
}

void camera_component::update(f32 delta)
{
    //if (!m_recalculate)
    //{
    //    return;
    //}

    //m_recalculate = false;

    constexpr math::vector front{ 0.f, 0.f, 1.f, 0.f };
    math::vector           look = XMVector3Transform(front, m_owner->transformation().rotation_matrix());
    const math::vector     up   = XMVector3Transform(m_owner->transformation().up(), m_owner->transformation().rotation_matrix());
    look                        = XMVectorAdd(m_owner->transformation().position_vector(), look);
    m_view                      = XMMatrixLookAtLH(m_owner->transformation().position_vector(), look, {0.f, 1.f, 0.f, 0.f});
}

void camera_component::add_to_engine()
{
    app::instance()->set_camera(this);
}
} // namespace yae