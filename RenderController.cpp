#include <windows.h>
#include <UIAnimation.h>
#include <vector>
#include <map>
#include <algorithm>
#include <memory>

#include "interfacemaphelpers.h"
#include "D2dIncludes.h"
#include "D2DResources.h"
#include "D2DNodes.h"
#include "D2DModel.h"

#include "RenderController.h"

RenderController::RenderController(HWND window_handle_in)
    : window_handle(window_handle_in)
    , animating(false)
    , in_render_loop(false)
    , animation_mgr_ptr(NULL)
    , animation_timer_ptr(NULL)
{
    HRESULT hr = CoCreateInstance(
        CLSID_UIAnimationManager,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&animation_mgr_ptr)
    );
    if (SUCCEEDED(hr))
    {
        // Create Animation Timer

        hr = CoCreateInstance(
            CLSID_UIAnimationTimer,
            NULL,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&animation_timer_ptr)
        );
        animation_mgr_ptr->SetManagerEventHandler(this);
    }

}

RenderController::~RenderController(void)
{
    ReleaseInterface(animation_mgr_ptr);
    ReleaseInterface(animation_timer_ptr);
}
void RenderController::SetWindow(HWND window_handle_in)
{
    window_handle = window_handle_in;
}
IUIAnimationManager* RenderController::GetAnimationManager()
{
    return animation_mgr_ptr;
}
IFACEMETHODIMP RenderController::QueryInterface(REFIID riid, void** ppvObject)
{
    if (ppvObject == NULL)
    {
        return E_POINTER;
    }

    *ppvObject = NULL;
    if (::IsEqualGUID(riid, IID_IUnknown) || ::IsEqualGUID(riid, IID_IUIAnimationManagerEventHandler))
    {
        *ppvObject = static_cast<IUIAnimationManagerEventHandler*>(this);
        return S_OK;
    }

    return E_NOINTERFACE;
}
IFACEMETHODIMP_(ULONG) RenderController::AddRef()
{
    return 1;
}
IFACEMETHODIMP_(ULONG) RenderController::Release()
{
    return 1;
}
IFACEMETHODIMP RenderController::OnManagerStatusChanged(__in UI_ANIMATION_MANAGER_STATUS new_status,
    __in UI_ANIMATION_MANAGER_STATUS previous_status)
{
    animating = false;
    if (UI_ANIMATION_MANAGER_BUSY == new_status)
    {
        animating = true;
        if (::IsWindow(window_handle))
        {
            ::InvalidateRect(window_handle, NULL, FALSE);
        }
    }
    return S_OK;
}
bool RenderController::Animating()
{
    //UI_ANIMATION_MANAGER_STATUS status ;
    //animation_mgr_ptr->GetStatus( &status ) ;
    //return  status == UI_ANIMATION_MANAGER_BUSY ? true : false ;
    return animating;
}
bool RenderController::RenderNodes(const std::vector< RenderedNode* >& nodes, D2DResources* d2d_objects_ptr)
{
    if (NULL == d2d_objects_ptr)
    {
        return false;
    }

    ID2D1DeviceContext1* device_context = d2d_objects_ptr->GetDeviceContext();

    if (!device_context)
    {
        return false;
    }

    UI_ANIMATION_SECONDS seconds_now;
    HRESULT hr = animation_timer_ptr->GetTime(&seconds_now);

    if (FAILED(hr))
    {
        return false;
    }

    hr = animation_mgr_ptr->Update(seconds_now);
    if (FAILED(hr))
    {
        return false;
    }

    device_context->BeginDraw();
    device_context->SetTransform(D2D1::Matrix3x2F::Identity());
    device_context->Clear(D2D1::ColorF(D2D1::ColorF::White));

    for (UINT i = 0; i < nodes.size(); i++)
    {
        nodes[i]->Render(d2d_objects_ptr, animation_mgr_ptr);
    }

    hr = device_context->EndDraw();
    if (D2DERR_RECREATE_TARGET == hr)
    {
        return true;
    }

    if (SUCCEEDED(hr))
    {
        auto swap_chain = d2d_objects_ptr->GetSwapChain();
        if (nullptr != swap_chain)
        {
            hr = swap_chain->Present(1, 0);
            if (DXGI_ERROR_DEVICE_REMOVED == hr || DXGI_ERROR_DEVICE_RESET == hr)
            {
                return true;
            }
        }
    }
    
    return false;
}

void RenderController::WindowDidResize(ID2DModel* model_ptr, float x, float y)
{
    std::vector<RenderedNode*> nodes = model_ptr->GetRenderedNodes();

    D2D_SIZE_F size{ (float)(x), (float)(y) };
    for (RenderedNode* node : nodes)
    {
        node->WindowDidResize(model_ptr->GetD2dResources(), size);
    }
}

void RenderController::WindowPaintReceived(ID2DModel* model_ptr, GuiHelpers::MessageLoop* msg_loop)
{
    //necessary before we call the run loop, otherwise we get another paint
    //and we recurse to death
    ::ValidateRect(window_handle, NULL);
    do
    {
        in_render_loop = true;
        bool recreate_target = RenderNodes(model_ptr->GetRenderedNodes(), model_ptr->GetD2dResources());
        if (recreate_target)
        {
            model_ptr->GetD2dResources()->Reset();
        }
        msg_loop->RunLocalLoopUntilEmpty();
    } while (Animating() && ::IsWindow(window_handle));
    in_render_loop = false;
}

void RenderController::RenderIsStale()
{
    if (!in_render_loop)
    {
        ::InvalidateRect(window_handle, NULL, FALSE);
    }
}