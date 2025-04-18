#pragma once
#include <vector>
#include "D2DResources.h"
#include "D2DNodes.h"
#include "CollisionObjects.h"

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
};
