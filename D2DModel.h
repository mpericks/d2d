#pragma once
#include <vector>
#include <UIAnimation.h>
#include "D2DResources.h"
#include "RenderedNode.h"
#include "CollisionObjects.h"

struct UIAnimationInterfaces
{
    IUIAnimationManager* animation_mgr;
    IUIAnimationTimer* animation_timer;
    IUIAnimationTransitionLibrary* animation_transition_library;
};

struct ID2DModel
{
    virtual ~ID2DModel() = default;
    virtual D2DResources* GetD2dResources() const = 0;
    virtual UIAnimationInterfaces GetAnimationInterfaces() const = 0;
    virtual HWND GetMainHWND() const = 0;
    virtual D2D1::ColorF GetBackgroundColor() const = 0;
};


