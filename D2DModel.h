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
    virtual D2DResources* GetD2dResources() = 0;
    virtual std::vector< RenderedNode* > GetRenderedNodes() = 0;
    virtual void AddRenderedNode(RenderedNode* new_node) = 0;
    virtual void RemoveRenderedNode(RenderedNode* remove_me) = 0;
    virtual std::vector<Neato::ICollidable*> GetColliders() = 0;
    virtual void CullCollisionNodes() = 0;
    virtual void AddCollider(Neato::ICollidable* collider) = 0;
    virtual void RemoveCollider(Neato::ICollidable* remove_me) = 0;
    virtual UIAnimationInterfaces GetAnimationInterfaces() = 0;
    virtual HWND GetMainHWND() = 0;
};
