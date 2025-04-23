#pragma once

#include <Windows.h>
#include <map>
#include <memory>
#include <d2dIncludes.h>
#include <wrl/client.h>

class D2DResources 
{
public:
  D2DResources(HWND window_handle);
  D2DResources(void) = delete;
  virtual ~D2DResources();

public:
  ID2D1Factory2* GetFactory() ;
  ID2D1DeviceContext1* GetDeviceContext() const;
  ID2D1Brush* GetBrush( const int brush_id )  const ;
  ID2D1Geometry* GetGeometry( const int geometry_id  ) const ;
  IDWriteTextFormat* GetTextFormat( const int format_id ) const ;
  IDXGISwapChain3* GetSwapChain() const;
  HRESULT Reset();
  void Resize(D2D_SIZE_F size);
  void SetBrush(int id, Microsoft::WRL::ComPtr<ID2D1Brush>  brush);
  void SetGeometry(int id, Microsoft::WRL::ComPtr<ID2D1Geometry> geometry);
  void SetTextFormat(int id, Microsoft::WRL::ComPtr<IDWriteTextFormat> text_format);

private:
  virtual HRESULT CreateMaps() = 0;
  void CleanMaps();
  HRESULT CreateDeviceContext();
  HRESULT CreateRenderTarget();
  HRESULT ResetDeviceContext();
  void CleanDeviceContext();
  HRESULT CleanRenderTarget();

private:
  std::map< int, Microsoft::WRL::ComPtr<ID2D1Brush> > brush_map ;
  std::map< int, Microsoft::WRL::ComPtr<ID2D1Geometry> > geometry_map ;
  std::map< int, Microsoft::WRL::ComPtr<IDWriteTextFormat>> text_format_map ;
  HWND associated_window_handle ;
  Microsoft::WRL::ComPtr<ID2D1Factory2> m_d2d_factory;
  Microsoft::WRL::ComPtr<ID2D1DeviceContext1> m_d2d_device_context;
  Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_d3d_command_queue;
  Microsoft::WRL::ComPtr<IDXGISwapChain3> m_dxgi_swap_chain;
  Microsoft::WRL::ComPtr<ID2D1Bitmap1> m_d2d_bitmap;
  Microsoft::WRL::ComPtr<ID3D11DeviceContext1> m_d3d11_device_context;
  Microsoft::WRL::ComPtr<ID3D11Debug> m_dx_debug;
};

D2DResources* CreateDirect2dResources( HWND window_handle ) ;