#include <windows.h>
#include <UIAnimation.h>
#include <vector>
#include <map>
#include <algorithm>
#include <memory>

#include "interfacemaphelpers.h"
#include "D2dIncludes.h"
#include "D2DResources.h"
#include "RenderedNode.h"
#include "D2DModel.h"
#include "ToobHelpers.h"

#include "RenderController.h"

RenderController::RenderController(ID2DModel* model_in)
    : m_model(model_in)
{
}

RenderController::~RenderController(void)
{

}

bool RenderController::RenderNodes()
{
    D2DResources* d2d_objects_ptr = m_model->GetD2dResources();

    ID2D1DeviceContext1* device_context = d2d_objects_ptr->GetDeviceContext();

    device_context->BeginDraw();
    device_context->SetTransform(D2D1::Matrix3x2F::Identity());
    device_context->Clear(D2D1::ColorF(D2D1::ColorF::White));

    for (RenderedNode* node : m_model->GetRenderedNodes())
    {
        node->Render();
    }

    HRESULT hr = device_context->EndDraw();
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

void RenderController::WindowDidResize(D2D_SIZE_F size)
{
    std::vector<RenderedNode*> nodes = m_model->GetRenderedNodes();

    for (RenderedNode* node : nodes)
    {
        node->WindowDidResize(size);
    }
}

void RenderController::UpdateNodes(bool force_all)
{
    UIAnimationInterfaces anime_interfaces = m_model->GetAnimationInterfaces();
    UI_ANIMATION_SECONDS time_now;
    HRESULT hr = anime_interfaces.animation_timer->GetTime(&time_now);
    UI_ANIMATION_UPDATE_RESULT result;
    hr = anime_interfaces.animation_mgr->Update(time_now, &result);
    if (UI_ANIMATION_UPDATE_RESULT::UI_ANIMATION_UPDATE_VARIABLES_CHANGED == result
        || force_all)
    {
        for (RenderedNode* node : m_model->GetRenderedNodes())
        {
            node->Update();
        }
    }

}
