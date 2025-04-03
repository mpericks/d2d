#pragma once
#include <D2DResources.h>
#include <vector>
#include <D2DNodes.h>

struct ID2DModel
{
    virtual ~ID2DModel() = default;
    virtual D2DResources* GetD2dResources() = 0;
    virtual std::vector< RenderedNode* > GetRenderedNodes() = 0;
    virtual void AddRenderedNode(RenderedNode* new_node) = 0;
    virtual void RemoveRenderedNode(RenderedNode* remove_me) = 0;
};
