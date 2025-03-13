#pragma once
#include <D2DResources.h>
#include <vector>
#include <D2DNodes.h>

struct ID2DModel
{
    virtual D2DResources* GetD2dResources() = 0;
    virtual std::vector< RenderedNode* > GetRenderedNodes() = 0;
    virtual ~ID2DModel() = default;
};

ID2DModel* CreateModel(HWND window_handle);