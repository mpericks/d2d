#include <windows.h>
#include <crtdbg.h>
#include <intsafe.h>
#include <d2dIncludes.h>

#include "InterfaceMapHelpers.h"
#include "D2DResources.h"

D2DResources::D2DResources(void) : D2DResources((HWND)NULL)
{
    _RPTF0(_CRT_ERROR, "Bad D2DResources ctor call. Need to send in an HWND");
}

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
    ClearInterfaceMap(&brush_map);
    ClearInterfaceMap(&geometry_map);
    ClearInterfaceMap(&text_format_map);
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
    if (m_d2d_device_context)
    {
        m_d2d_device_context->SetTarget(nullptr);
    }
    m_d2d_bitmap.Reset();
    return S_OK;
}
void D2DResources::SetBrushMap(std::map< int, ID2D1Brush* > brush_map_init)
{
    brush_map = brush_map_init;
}
void D2DResources::SetGeometryMap(std::map< int, ID2D1Geometry* > geometry_map_init)
{
    geometry_map = geometry_map_init;
}
void D2DResources::SetTextFormatMap(std::map< int, IDWriteTextFormat*> new_text_format_map)
{
    text_format_map = new_text_format_map;
}

ID2D1DeviceContext1* D2DResources::GetDeviceContext() const
{
    return m_d2d_device_context.Get();
}
ID2D1Brush* D2DResources::GetBrush(const int brush_id) const
{
    ID2D1Brush* brush_ptr = brush_map.find(brush_id)->second;
    if (NULL == brush_ptr)
    {
        throw std::exception("Can't find brush ");
    }
    return  brush_ptr;
}
ID2D1Geometry* D2DResources::GetGeometry(const int geometry_id) const
{
    ID2D1Geometry* geometry_ptr = geometry_map.find(geometry_id)->second;
    if (NULL == geometry_ptr)
    {
        throw std::exception("Can't find geometry ");
    }
    return geometry_ptr;
}
IDWriteTextFormat* D2DResources::GetTextFormat(const int format_id) const
{
    IDWriteTextFormat* format_ptr = text_format_map.find(format_id)->second;
    if (NULL == format_ptr)
    {
        throw std::exception("Can't find text format ");
    }
    return format_ptr;
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

    //ID3D12Device* d3d12_base_device = nullptr;
    //IUnknown* unknown_swap_chain_device = nullptr;

    Microsoft::WRL::ComPtr<IDXGIFactory4> dxgi_factory;
    CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&dxgi_factory));

    Microsoft::WRL::ComPtr<ID3D11DeviceContext> base_device_context;
    Microsoft::WRL::ComPtr<ID3D11Device> d3d11_base_device;
    Microsoft::WRL::ComPtr<IUnknown> iunknown_device;

    //hr = D3D12CreateDevice(
    //                        0,
    //                        D3D_FEATURE_LEVEL_11_1,
    //                        IID_PPV_ARGS(&d3d12_base_device)
    //                        );
    //if (FAILED(hr))
    //{
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

        hr = D3D11CreateDevice(
                                0,
                                D3D_DRIVER_TYPE_HARDWARE,
                                0,
                                creation_flags,
                                feature_levels,
                                ARRAYSIZE(feature_levels),
                                D3D11_SDK_VERSION,
                                &d3d11_base_device,
                                0,
                                &base_device_context
                               );
        if (FAILED(hr))
        {
            _RPTF0(_CRT_ERROR, "D3D11CreateDevice failed");
            return hr;
        }
        //d3d11_base_device.As(&debug_ptr);
        d3d11_base_device.As(&iunknown_device);
    //}
    //else
    //{
    //    // Describe and create the command queue.
    //    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    //    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    //    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    //    ID3D12CommandQueue* command_queue = nullptr;
    //    hr = d3d12_base_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&command_queue));
    //    if (FAILED(hr))
    //    {
    //        _RPTF0(_CRT_ERROR, "CreateCommandQueue failed");
    //        return hr;
    //    }
    //    unknown_swap_chain_device = command_queue;
    //    mp_d3d_command_queue = UniqueComPtr(command_queue);
    //}

    /*if (nullptr == d3d11_base_device)
    {
        hr = D3D11On12CreateDevice(
            d3d12_base_device,
            d3d11DeviceFlags,
            nullptr,
            0,
            reinterpret_cast<IUnknown**>(&mp_d3d_command_queue),
            1,
            0,
            &d3d11_base_device,
            &base_device_context,
            nullptr
        );
        if (FAILED(hr))
        {
            _RPTF0(_CRT_ERROR, "D3D11On12CreateDevice failed");
            return hr;
        }
    }*/

    hr = base_device_context.As(&m_d3d11_device_context);
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

    Microsoft::WRL::ComPtr<IDXGISwapChain1> swap_chain1;
    hr = dxgi_factory->CreateSwapChainForHwnd(
                                                iunknown_device.Get(),
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

    //hr = dxgi_factory->MakeWindowAssociation(associated_window_handle, DXGI_MWA_NO_ALT_ENTER);

    Microsoft::WRL::ComPtr<IDXGIDevice> dxgi_device;
    d3d11_base_device.As(&dxgi_device);

    Microsoft::WRL::ComPtr<ID2D1Device1> d2d1_device;
    hr = m_d2d_factory->CreateDevice(dxgi_device.Get(), &d2d1_device);
    if (FAILED(hr))
    {
        _RPTF0(_CRT_ERROR, "CreateDevice on the D2D factory failed");
        return hr;
    }

 
    hr = d2d1_device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &m_d2d_device_context);
    if (FAILED(hr))
    {
        _RPTF0(_CRT_ERROR, "CreateDeviceCOntext on the D2D device failed");
        return hr;
    }

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

