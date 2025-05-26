#pragma once

#include "D2DModel.h"

class RenderController
{
public:
    RenderController(ID2DModel* model_in);
    ~RenderController(void);
public:
    void UpdateNodes(std::vector<RenderedNode*>& nodes, bool force_all = false);
    bool RenderNodes(std::vector<RenderedNode*>& nodes);
    void WindowDidResize(D2D_SIZE_F new_size, std::vector<RenderedNode*>& nodes);
private:
    ID2DModel* m_model;
};
