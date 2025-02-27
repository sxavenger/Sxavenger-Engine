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
#include <array>
#include <deque>

////////////////////////////////////////////////////////////////////////////////////////////
// Performance class
////////////////////////////////////////////////////////////////////////////////////////////
class Performance
	: public ISystemDebugGui {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////

	using LapContainer = std::deque<std::pair<std::string, TimePointf<TimeUnit::millisecond>>>;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Begin();

	void End();

	void RecordLap(const std::string& name);

	void SystemDebugGui() override;

	template <TimeUnit T = TimeUnit::second>
	TimePointf<T> GetDeltaTime() const { return runtime_.GetDeltaTime<T>(); }

	const LapContainer& GetLap() const { return laps_[(lapIndex_ + 1) % lapCount_]; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* run time tracker *//

	RunTimeTracker runtime_;

	//* parameter *//

	float limitFrame_ = 60.0f;

	//* lap *//

	static const uint8_t lapCount_ = 2;
	std::array<LapContainer, lapCount_> laps_;

	uint8_t lapIndex_ = 0;

	TimePointf<TimeUnit::second> recordInterval_ = 1.0f;
	TimePointf<TimeUnit::second> recordedTime_   = {};
	bool isRecord_ = true;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void WaitFrame() const;

};
