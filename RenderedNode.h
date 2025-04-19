#pragma once

struct D2D_SIZE_F;

struct RenderedNode
{
	virtual ~RenderedNode() = default;
	virtual void Render() = 0;
	virtual void Update() = 0;
	virtual void WindowDidResize(const D2D_SIZE_F& new_size) = 0;
};