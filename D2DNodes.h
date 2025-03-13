#pragma once

#include <bitset>
#include <UIAnimation.h>
#include <vector>

class D2DResources;
struct ControllerVisitor;
struct IUIAnimationManager;
struct IUIAnimationTimer;
struct IUIAnimationTransitionLibrary;

struct RenderedNode
{
	virtual bool IsAnimating() const = 0;
	virtual HRESULT Render(D2DResources* d2d_objects_ptr, IUIAnimationManager* animation_mgr_ptr) = 0;
	//virtual void AcceptControllerVisitor(ControllerVisitor* controller_ptr) = 0;
};

class RenderedNodeBaseImpl : public RenderedNode
{
public:
	RenderedNodeBaseImpl();
	~RenderedNodeBaseImpl();
	void SetOneState(int state_facet);
	void ClearOneState(int state_facet);
	bool IsOneStateSet(int state_facet) const;
	UI_ANIMATION_SECONDS GetStateChangeTime(int state_facet) const;
private:
	virtual void StateDidChange(int facet, bool on_or_off) {};
protected:
	IUIAnimationTimer* animation_timer_ptr;
	IUIAnimationTransitionLibrary* transition_library_ptr;
private:
	std::bitset<128> state_bits;
	std::vector< UI_ANIMATION_SECONDS > state_change_times;
};

//std::vector< RenderedNode* > CreateRenderedNodes();