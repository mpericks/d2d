#include <windows.h>
#include <algorithm>
#include <crtdbg.h>
#include <intsafe.h>

#include "d2dIncludes.h"

#include "D2DResources.h"

using Microsoft::WRL::ComPtr;

D2DResources::D2DResources(HWND window_handle)
    : associated_window_handle(window_handle)
    , m_d2d_factory(nullptr)
    , m_d2d_device_context(nullptr)
    , m_d3d_command_queue(nullptr)
    , m_dxgi_swap_chain(nullptr)
    , m_d2d_bitmap(nullptr)
    , m_d3d11_device_context(nullptr)
    , m_dx_debug(nullptr)
{
    HRESULT hr = S_OK;

    // Create a Direct2D factory.
    hr = D2D1CreateFactory<ID2D1Factory2>(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_d2d_factory);

    if (SUCCEEDED(hr))
    {
        hr = CreateDeviceContext();
    }
    if (FAILED(hr))
    {
        throw hr;
    }
}
D2DResources::~D2DResources(void)
{
    CleanMaps();
    CleanDeviceContext();
    if (m_dx_debug)
    {
        m_dx_debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
        m_dx_debug.Reset();
    }
}
ID2D1Factory2* D2DResources::GetFactory()
{
    return m_d2d_factory.Get();
}
void D2DResources::CleanMaps()
{
    std::for_each(brush_map.begin(), brush_map.end(), [](std::pair<int, ComPtr<ID2D1Brush>> pair) {
        pair.second.Reset();
        });
  
    std::for_each(geometry_map.begin(), geometry_map.end(), [](std::pair<int, ComPtr<ID2D1Geometry>> pair) {
        pair.second.Reset();
        });

    std::for_each(text_format_map.begin(), text_format_map.end(), [](std::pair<int, ComPtr<IDWriteTextFormat>> pair) {
        pair.second.Reset();
        });
}
HRESULT D2DResources::ResetDeviceContext()
{
    CleanDeviceContext();
    return CreateDeviceContext();
}
void D2DResources::CleanDeviceContext()
{
    CleanRenderTarget();
    m_d2d_device_context.Reset();

    m_dx_debug.Reset();
    m_d3d_command_queue.Reset();
    m_dxgi_swap_chain.Reset();
    if (m_d3d11_device_context)
    {
        m_d3d11_device_context->ClearState();
        m_d3d11_device_context->Flush();
        m_d3d11_device_context.Reset();
    }
}
HRESULT D2DResources::CleanRenderTarget()
{
    m_d2d_bitmap.Reset();   
    if (m_d2d_device_context)
    {
        m_d2d_device_context->SetTarget(nullptr);
    }

    return S_OK;
}

struct CreateD3D11DeviceReturn
{
    HRESULT hr;
    ComPtr<ID3D11Device> d3d11_base_device;
    ComPtr<ID3D11DeviceContext> base_device_context;
    ComPtr<IUnknown> iunknown_device;
};

CreateD3D11DeviceReturn CreateD3D11Device()
{
    CreateD3D11DeviceReturn ret_val;

    UINT creation_flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(_DEBUG)
    // If the project is in a debug build, enable the debug layer.
    creation_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    // couldn't make a D3D12 device. Try to make an 11
    D3D_FEATURE_LEVEL feature_levels[]{
                                D3D_FEATURE_LEVEL_11_1,
                                D3D_FEATURE_LEVEL_11_0,
                                D3D_FEATURE_LEVEL_10_1,
                                D3D_FEATURE_LEVEL_10_0,
                                D3D_FEATURE_LEVEL_9_3,
                                D3D_FEATURE_LEVEL_9_2,
                                D3D_FEATURE_LEVEL_9_1,
                                };

    ret_val.hr = D3D11CreateDevice(
                                    0,
                                    D3D_DRIVER_TYPE_HARDWARE,
                                    0,
                                    creation_flags,
                                    feature_levels,
                                    ARRAYSIZE(feature_levels),
                                    D3D11_SDK_VERSION,
                                    &ret_val.d3d11_base_device,
                                    0,
                                    &ret_val.base_device_context
                                );

    if (FAILED(ret_val.hr))
    {
        _RPTF0(_CRT_ERROR, "D3D11CreateDevice failed");
        return ret_val;
    }
    //d3d11_base_device.As(&m_dx_debug);
    ret_val.d3d11_base_device.As(&ret_val.iunknown_device);

    return ret_val;
}

//CreateD3D11DeviceReturn CreateD3D11On12Device()
//{
//    CreateD3D11DeviceReturn ret_val;
//    ComPtr<ID3D12Device> d3d12_base_device;
//    ret_val.hr = D3D12CreateDevice(
//                                    0,
//                                    D3D_FEATURE_LEVEL_11_1,
//                                    IID_PPV_ARGS(&ret_val.d3d12_base_device)
//                                    );
//    if (SUCCEEDED(ret_val.hr))
//    {
//        // Describe and create the command queue.
//        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
//        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
//        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
//        hr = d3d12_base_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_d3d_command_queue));
//        if (FAILED(hr))
//        {
//            _RPTF0(_CRT_ERROR, "CreateCommandQueue failed");
//            return hr;
//        }
//        m_d3d_command_queue.As(&iunknown_device);
//    }
//
//    if (SUCCEEDED(hr))
//    {
//        UINT creation_flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
//#if defined(_DEBUG)
//        // If the project is in a debug build, enable the debug layer.
//        creation_flags |= D3D11_CREATE_DEVICE_DEBUG;
//#endif
//        ComPtr<IUnknown> iunknown_12_device;
//        d3d12_base_device.As(&iunknown_12_device);
//        hr = D3D11On12CreateDevice(
//                                    iunknown_12_device.Get(),
//                                    creation_flags,
//                                    nullptr,
//                                    0,
//                                    &iunknown_device,
//                                    1,
//                                    0,
//                                    &d3d11_base_device,
//                                    &base_device_context,
//                                    nullptr
//                                   );
//        if (FAILED(hr))
//        {
//            _RPTF0(_CRT_ERROR, "D3D11On12CreateDevice failed");
//            return hr;
//        }
//    }
//
//    return hr;
//}
void D2DResources::SetBrush(int id, ComPtr<ID2D1Brush> brush)
{
    brush_map[id] = brush;
}
void D2DResources::SetGeometry(int id, ComPtr<ID2D1Geometry> geometry)
{
    geometry_map[id] = geometry;
}
void D2DResources::SetTextFormat(int id, ComPtr<IDWriteTextFormat> text_format)
{
    text_format_map[id] = text_format;
}

ID2D1DeviceContext1* D2DResources::GetDeviceContext() const
{
    return m_d2d_device_context.Get();
}
ID2D1Brush* D2DResources::GetBrush(const int brush_id) const
{
    ComPtr<ID2D1Brush> brush_ptr = brush_map.find(brush_id)->second;
    if (NULL == brush_ptr)
    {
        _RPT1(_CRT_ERROR, "brush %d doesn't exist", brush_id);
        throw std::exception("Can't find brush ");
    }
    return  brush_ptr.Get();
}
ID2D1Geometry* D2DResources::GetGeometry(const int geometry_id) const
{
    ComPtr<ID2D1Geometry> geometry_ptr = geometry_map.find(geometry_id)->second;
    if (NULL == geometry_ptr)
    {
        _RPT1(_CRT_ERROR, "geometry %d doesn't exist", geometry_id);
        throw std::exception("Can't find geometry ");
    }
    return geometry_ptr.Get();
}
IDWriteTextFormat* D2DResources::GetTextFormat(const int format_id) const
{
    ComPtr<IDWriteTextFormat> format_ptr = text_format_map.find(format_id)->second;
    if (NULL == format_ptr)
    {
        _RPT1(_CRT_ERROR, "text format %d doesn't exist", format_id);
        throw std::exception("Can't find text format ");
    }
    return format_ptr.Get();
}
IDXGISwapChain3* D2DResources::GetSwapChain() const
{
    return m_dxgi_swap_chain.Get();
}
HRESULT D2DResources::CreateDeviceContext()
{
    HRESULT hr = S_OK;

    UINT dxgiFactoryFlags = 0;
    UINT d3d11DeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    D2D1_FACTORY_OPTIONS d2dFactoryOptions = {};

    ComPtr<IDXGIFactory4> dxgi_factory;
    CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&dxgi_factory));

    CreateD3D11DeviceReturn create_result = CreateD3D11Device();

    if (FAILED(create_result.hr))
    {
        return create_result.hr;
    }

    hr = create_result.base_device_context.As(&m_d3d11_device_context);
    if (FAILED(hr))
    {
        return hr;
    }

    // Describe the swap chain.
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = 2;
    swapChainDesc.Width = 0;
    swapChainDesc.Height = 0;
    swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    swapChainDesc.SampleDesc.Count = 1;

    ComPtr<IDXGISwapChain1> swap_chain1;
    hr = dxgi_factory->CreateSwapChainForHwnd(
                                                create_result.iunknown_device.Get(),
                                                associated_window_handle,
                                                &swapChainDesc,
                                                nullptr,
                                                nullptr,
                                                &swap_chain1
                                              );
    if (FAILED(hr))
    {
        _RPTF0(_CRT_ERROR, "CreateSwapChainForHwnd failed");
        return hr;
    }

    swap_chain1.As(&m_dxgi_swap_chain);

    ComPtr<IDXGIDevice> dxgi_device;
    create_result.d3d11_base_device.As(&dxgi_device);

    ComPtr<ID2D1Device1> d2d1_device;
    hr = m_d2d_factory->CreateDevice(dxgi_device.Get(), &d2d1_device);
    if (FAILED(hr))
    {
        _RPTF0(_CRT_ERROR, "CreateDevice on the D2D factory failed");
        return hr;
    }

 
    hr = d2d1_device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &m_d2d_device_context);
    if (FAILED(hr))
    {
        _RPTF0(_CRT_ERROR, "CreateDeviceContext on the D2D device failed");
        return hr;
    }
    d2d1_device.Reset();

    hr = CreateRenderTarget();

    return hr;
}

HRESULT D2DResources::CreateRenderTarget()
{
    if (m_dxgi_swap_chain == nullptr)
    {
        return E_UNEXPECTED;
    }

    Microsoft::WRL::ComPtr<IDXGISurface> dxgi_surface;
    HRESULT hr = m_dxgi_swap_chain->GetBuffer(0, IID_PPV_ARGS(&dxgi_surface));
    if (FAILED(hr))
    {
        _RPTF0(_CRT_ERROR, "GetBuffer, on the swap chain failed");
        return hr;
    }

    UINT dpi = GetDpiForWindow(associated_window_handle);
    D2D1_BITMAP_OPTIONS bm_options = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
    D2D1_PIXEL_FORMAT pixel_format = D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE);
    D2D1_BITMAP_PROPERTIES1 bm_props = D2D1::BitmapProperties1(
        bm_options,
        pixel_format,
        (float)dpi,
        (float)dpi
    );

    hr = m_d2d_device_context->CreateBitmapFromDxgiSurface(
        dxgi_surface.Get(),
        &bm_props,
        m_d2d_bitmap.ReleaseAndGetAddressOf()
    );

    if (FAILED(hr))
    {
        _RPTF0(_CRT_ERROR, "CreateBitmapFromDxgiSurface failed");
        return hr;
    }
    m_d2d_device_context->SetTarget(m_d2d_bitmap.Get());
    dxgi_surface.Reset();

    return hr;
}
HRESULT D2DResources::Reset()
{
    CleanMaps();
    HRESULT hr = ResetDeviceContext();
    if (SUCCEEDED(hr))
    {;
        hr = CreateMaps();
    }
    return hr;
}

void D2DResources::Resize()
{
    if (m_d3d11_device_context)
    {
        m_d3d11_device_context->ClearState();
        m_d3d11_device_context->Flush();
    }
    CleanMaps();
    CleanRenderTarget();
    HRESULT hr = m_dxgi_swap_chain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
    hr = CreateRenderTarget();
    if (SUCCEEDED(hr))
    {
        CreateMaps();
    }
}

