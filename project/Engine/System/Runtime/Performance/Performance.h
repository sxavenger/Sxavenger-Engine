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
//! @brief パフォーマンス計測を行うクラス.
class Performance
	: public ISystemDebugGui {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////

	using LapContainer = std::deque<std::pair<std::string, TimePointd<TimeUnit::millisecond>>>;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Begin();

	void End();

	void RecordLap(const std::string& name);

	void SystemDebugGui() override;

	template <TimeUnit T = TimeUnit::second>
	TimePointd<T> GetDeltaTime() const { return runtime_.GetDeltaTime<T>(); }

	const LapContainer& GetLap() const { return laps_[(lapIndex_ + 1) % lapCount_]; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* run time tracker *//

	RunTimeTracker runtime_;

	//* lap *//

	static const uint8_t lapCount_ = 2;
	std::array<LapContainer, lapCount_> laps_;

	uint8_t lapIndex_ = 0;

	TimePointd<TimeUnit::second> recordInterval_ = 1.0f;
	TimePointd<TimeUnit::second> recordedTime_   = {};
	bool isRecord_ = true;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void WaitFrame() const;

};
