#pragma once

#include "D2DModel.h"

class RenderController
{
public:
    RenderController(ID2DModel* model_in);
    ~RenderController(void);
public:
    void UpdateNodes(bool force_all = false);
    bool RenderNodes();
    void WindowDidResize(D2D_SIZE_F new_size);
private:
    ID2DModel* m_model;
};
