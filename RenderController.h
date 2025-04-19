#pragma once

#include "D2DModel.h"

class RenderController
{
public:
    RenderController(ID2DModel* model_in);
    ~RenderController(void);
public:
    void UpdateNodes();
    bool RenderNodes();
    void WindowDidResize();
private:
    ID2DModel* m_model;
    bool animating;
    bool in_render_loop;
};
