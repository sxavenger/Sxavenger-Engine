#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* performance
#include "TimePoint.h"
#include "RunTimeTracker.h"

//* engine
#include <Engine/System/UI/ISystemDebugGui.h>

//* c++
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////
// Performance class
////////////////////////////////////////////////////////////////////////////////////////////
class Performance
	: public ISystemDebugGui {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Begin();

	void End();

	void SystemDebugGui() override;

	TimePointf<TimeUnit::second> GetDeltaTime() const { return runtime_.GetDeltaTime<TimeUnit::second>(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* run time tracker *//

	RunTimeTracker runtime_;

	//* parameter *//

	float limitFrame_ = 60.0f;

	std::optional<TimePointf<TimeUnit::millisecond>> fixedDeltaTime_ = std::nullopt;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void WaitFrame() const;

};
