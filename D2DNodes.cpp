#include <windows.h>
#include <UIAnimation.h>
#include <vector>
#include <bitset>
#include <WTypesbase.h>
#include <combaseapi.h>
#include <exception>

#include "InterfaceMapHelpers.h"

#include "D2DNodes.h"


RenderedNodeBaseImpl::RenderedNodeBaseImpl(): animation_timer_ptr( NULL ), transition_library_ptr( NULL ) , state_bits( 0 ), state_change_times( 128, 0 )
{
	HRESULT hr = ::CoCreateInstance(
									CLSID_UIAnimationTimer,
									NULL,
									CLSCTX_INPROC_SERVER,
									IID_PPV_ARGS(&animation_timer_ptr)
									);
	if ( NULL == animation_timer_ptr )
	{
		throw std::exception( "Trouble creating animation components in Render base implementation", hr ) ;
	}
    hr = ::CoCreateInstance(
							CLSID_UIAnimationTransitionLibrary,
							NULL,
							CLSCTX_INPROC_SERVER,
							IID_PPV_ARGS(&transition_library_ptr)
							);
	if ( NULL == transition_library_ptr )
	{
		throw std::exception( "Trouble creating animation components in Render base implementation", hr ) ;
	}

}
RenderedNodeBaseImpl::~RenderedNodeBaseImpl() 
{
	ReleaseInterface( transition_library_ptr ) ;
	ReleaseInterface( animation_timer_ptr ) ;
}

void RenderedNodeBaseImpl::SetOneState( int state_facet ) 
{
  state_bits.set( state_facet ) ;
  UI_ANIMATION_SECONDS time_now ;
  if (SUCCEEDED(animation_timer_ptr->GetTime( &time_now )))
  {
	  state_change_times[state_facet] = time_now;
  }
  StateDidChange(state_facet, true);
}
void RenderedNodeBaseImpl::ClearOneState( int state_facet ) 
{
  state_bits.set( state_facet, false ) ;
  UI_ANIMATION_SECONDS time_now ;
  if (SUCCEEDED(animation_timer_ptr->GetTime(&time_now)))
  {
	  state_change_times[state_facet] = time_now;
  }
  StateDidChange(state_facet, false);
}
bool RenderedNodeBaseImpl::IsOneStateSet( int state_facet )  const
{
  return state_bits.test( state_facet ) ;
}

UI_ANIMATION_SECONDS RenderedNodeBaseImpl::GetStateChangeTime( int state_facet ) const
{
  return state_change_times.at( state_facet ) ;
}