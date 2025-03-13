#include <windows.h>
#include <windowsX.h>
#include <string>
#include <basetsd.h>
#include <d2d1.h>
#include <d2d1helper.h>

#include "GuiHelpers.h"
#include "D2DResources.h"
#include "D2DGenericModel.h"
#include "RenderController.h"
#include "KeyboardInputController.h"
#include "MouseInputController.h"

#include "D2DApplicationWindow.h"


D2DApplicationWindow::D2DApplicationWindow(GuiHelpers::MessageLoop* message_loop_ptr_init)
    : message_loop_ptr(message_loop_ptr_init)
    , in_render_loop(false)
    , render_controller()
{
    std::wstring class_name = L"Generic D2D";
    GuiHelpers::CreateDefaultMainWindow(this, class_name);
}

D2DApplicationWindow::~D2DApplicationWindow(void)
{

}
void D2DApplicationWindow::OnDestroy(HWND hWnd)
{
    ::PostQuitMessage(0);
}
LRESULT D2DApplicationWindow::OnCreate(LPCREATESTRUCT create_info_ptr)
{
    d2d_model.SetWindowHandle(Handle());
    message_loop_ptr->AddDialogWindow(Handle());
    render_controller.SetWindow(Handle());
    return TRUE;
}
LRESULT D2DApplicationWindow::OnGetMinMaxSize(HWND window_handle, LPMINMAXINFO min_max_info)
{
    UINT dpi = ::GetDpiForWindow(window_handle);
    min_max_info->ptMinTrackSize.x = (LONG)(640.0f * ((float)dpi / 96.0f));
    min_max_info->ptMinTrackSize.y = (LONG)(480.0f * ((float)dpi / 96.0f));
    return 0;
}
LRESULT D2DApplicationWindow::OnSize(HWND window_handle, UINT state, int width, int height)
{
    D2DResources* d2d_objects_ptr = d2d_model.GetD2dResources();
    if (NULL == d2d_objects_ptr)
    {
        return 0;
    }
    UINT dpi = ::GetDpiForWindow(Handle());
    ID2D1HwndRenderTarget* render_target = d2d_objects_ptr->GetRenderTarget();
    if (render_target)
    {
        render_target->Resize(D2D1::SizeU((UINT32)width, (UINT32)height));
    }
    return 0;
}
void D2DApplicationWindow::OnPaint(HWND window_handle)
{
    D2DResources* d2d_objects_ptr = d2d_model.GetD2dResources();
    if (NULL == d2d_objects_ptr)
    {
        return;
    }

    //necessary before we call the run loop, otherwise we get another paint
    //and we recurse to death
    ::ValidateRect(Handle(), NULL);
    do
    {
        in_render_loop = true;
        HRESULT hr = render_controller.RenderNodes(d2d_model.GetRenderedNodes(), d2d_model.GetD2dResources());
        if (hr == D2DERR_RECREATE_TARGET)
        {
            d2d_objects_ptr->Reset();
        }
        message_loop_ptr->RunLocalLoopUntilEmpty();
    } while (render_controller.Animating() && ::IsWindow(window_handle));
    in_render_loop = false;

}
void D2DApplicationWindow::RenderIsStale()
{
    if (!in_render_loop)
    {
        ::InvalidateRect(Handle(), NULL, FALSE);
    }
}
void D2DApplicationWindow::OnKeyDown(HWND window_handle, UINT virtual_key, BOOL is_down_already, int repeat_count, UINT flags)
{
    KeyboardInputController controller(virtual_key);
    bool significant = controller.VisitNodes(d2d_model.GetKeyboardableNodes());
    if (significant)
    {
        this->RenderIsStale();
    }
}
void D2DApplicationWindow::OnMouseLeftButtonDown(HWND window_handle, BOOL is_double_click, int x, int y, UINT_PTR keys_down)
{
    D2DResources* d2d_objects_ptr = d2d_model.GetD2dResources();
    if (NULL == d2d_objects_ptr)
    {
        return;
    }
    MouseInputController controller;
    bool significant = controller.VisitNodes(d2d_model.GetMousableNodes(), d2d_objects_ptr, x, y, WM_LBUTTONDOWN, keys_down);
    if (significant)
    {
        this->RenderIsStale();
    }
}
UINT D2DApplicationWindow::OnGetDlgCode(HWND window_handle, LPMSG msg_ptr)
{
    return DLGC_WANTARROWS;
}
LRESULT D2DApplicationWindow::MessageHandler(HWND window_handle, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT nRet = FALSE;
    switch (msg)
    {
        HANDLE_MSG(Handle(), WM_DESTROY, OnDestroy);
        HANDLE_MSG(Handle(), WM_SIZE, OnSize);
        HANDLE_MSG(Handle(), WM_PAINT, OnPaint);
        HANDLE_MSG(Handle(), WM_KEYDOWN, OnKeyDown);
        HANDLE_MSG(Handle(), WM_GETDLGCODE, OnGetDlgCode);
        HANDLE_MSG(Handle(), WM_GETMINMAXINFO, OnGetMinMaxSize);
    case  WM_LBUTTONDOWN:
        OnMouseLeftButtonDown(Handle(), FALSE, (int)(short)GET_X_LPARAM(lParam), (int)(short)GET_Y_LPARAM(lParam), (UINT)(wParam));
    default:
        nRet = DefWindowProc(Handle(), msg, wParam, lParam);
        break;
    }
    return nRet;
}