#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Foundation.h>

//* lib
#include <Lib/Adapter/Time/TimePoint.h>
#include <Lib/Adapter/Time/RunTimeTracker.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// Performance class
////////////////////////////////////////////////////////////////////////////////////////////
class Performance {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Begin();

	void End();

	template <TimeUnit T = TimeUnit::second>
	TimePointd<T> GetDeltaTimed() const { return runtime_.GetDeltaTime<T>(); }

	template <TimeUnit T = TimeUnit::second>
	TimePointf<T> GetDeltaTimef() const { return runtime_.GetDeltaTime<T>(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* run time tracker *//

	RunTimeTracker runtime_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void WaitFrame() const;

};

SXAVENGER_ENGINE_NAMESPACE_END
