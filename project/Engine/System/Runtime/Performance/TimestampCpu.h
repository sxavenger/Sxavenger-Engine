#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Foundation.h>

//* lib
#include <Lib/Adapter/Time/TimePoint.h>
#include <Lib/Adapter/Time/RunTimeTracker.h>

//* c++
#include <array>
#include <list>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// TimestampCpu class
////////////////////////////////////////////////////////////////////////////////////////////
class TimestampCpu {
public:

	//////////////////////////////////////////////////////////////////////////////////////////
	// Stamp structure
	//////////////////////////////////////////////////////////////////////////////////////////
	struct Stamp {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void CalculateDelta(const TimePointd<TimeUnit::millisecond>& previous);

		//=========================================================================================
		// public variables
		//=========================================================================================

		std::string name;
		TimePointd<TimeUnit::millisecond> elapsed;
		TimePointd<TimeUnit::millisecond> delta;

	};

	//////////////////////////////////////////////////////////////////////////////////////////
	// using
	//////////////////////////////////////////////////////////////////////////////////////////
	using Timestamp = std::list<Stamp>;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Begin();

	void End();

	void Record(const std::string& name);

	const Timestamp& GetTimestamp() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* run time tracker *//

	RunTimeTracker runtime_;

	//* lap parameter *//

	static const uint8_t kTimestampCount = 2;
	std::array<Timestamp, kTimestampCount> timestamps_;

	uint8_t currentIndex_ = 0;

	TimePointd<TimeUnit::second> recordInterval_ = 1.0f;
	TimePointd<TimeUnit::second> recordedTimer_  = {};
	bool isRecord_ = true;

};

SXAVENGER_ENGINE_NAMESPACE_END


