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
  virtual ~D2DResources();
protected:
  D2DResources(void) ;
public:
  ID2D1Factory2* GetFactory() ;
  ID2D1DeviceContext1* GetDeviceContext() const;
  ID2D1Brush* GetBrush( const int brush_id )  const ;
  ID2D1Geometry* GetGeometry( const int geometry_id  ) const ;
  IDWriteTextFormat* GetTextFormat( const int format_id ) const ;
  IDXGISwapChain3* GetSwapChain() const;
  HRESULT Reset();
  void Resize();
private:
  void CleanMaps();
  HRESULT CreateDeviceContext();
  HRESULT CreateRenderTarget();
private:
  virtual HRESULT CreateMaps() = 0;
  HRESULT ResetDeviceContext();
  void CleanDeviceContext();
  HRESULT CleanRenderTarget();
protected:
  void SetBrushMap( std::map< int, ID2D1Brush* > brush_map ) ;
  void SetGeometryMap( std::map< int, ID2D1Geometry* > geometry_map ) ;
  void SetTextFormatMap( std::map< int, IDWriteTextFormat*> new_text_format_map ) ;
private:
  std::map< int, ID2D1Brush* > brush_map ;
  std::map< int, ID2D1Geometry* > geometry_map ;
  std::map< int, IDWriteTextFormat*> text_format_map ;
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