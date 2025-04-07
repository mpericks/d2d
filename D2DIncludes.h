#pragma once

#include <dxgi.h>
#include <dxgi1_4.h>
#include <d3d11on12.h>
#include <d3d11_2.h>
#include <d2d1.h>
#include <d2d1_3.h>
#include <d2d1helper.h>
#include <d2d1_3helper.h>
#include <dwrite.h>
#include <wincodec.h>
#include <cmath>
#include <numbers>

namespace Neato
{
    constexpr float rads_to_degrees_factor = (float)(180.0 / std::numbers::pi);
    constexpr float degrees_to_rads_factor = (float)(std::numbers::pi / 180.0);
}