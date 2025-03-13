#pragma once

#include <UIAnimation.h>
#include "D2DIncludes.h"
#include "GuiHelpers.h"
#include "D2DModel.h"

struct RenderedNode ;
class D2DResources ;
struct IUIAnimationManager ;
struct IUIAnimationTimer ;

class RenderController : public IUIAnimationManagerEventHandler
{
public:
  RenderController(HWND window_handle_in);
  ~RenderController(void);
public:
  bool Animating() ;
  void RenderIsStale();
  bool RenderNodes( const std::vector< RenderedNode* >& nodes, D2DResources* object_map );
  void WindowPaintReceived(ID2DModel* model_ptr, GuiHelpers::MessageLoop* msg_loop);
  void SetWindow( HWND window_handle ) ;
  IFACEMETHODIMP OnManagerStatusChanged(
                                         __in UI_ANIMATION_MANAGER_STATUS new_status, 
                                         __in UI_ANIMATION_MANAGER_STATUS previous_status     
                                        ) ;
  IFACEMETHODIMP QueryInterface ( REFIID riid, void **ppvObject ) ;
  IFACEMETHODIMP_(ULONG) AddRef() ;
  IFACEMETHODIMP_(ULONG) Release() ;

private:
  HWND window_handle ;
  bool animating ;
  bool in_render_loop;
  IUIAnimationManager* animation_mgr_ptr ;
  IUIAnimationTimer* animation_timer_ptr;
};
