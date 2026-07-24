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
//! @brief CPU側の区間ごとの経過時間を計測するプロファイラ
class TimestampCpu {
public:

	//////////////////////////////////////////////////////////////////////////////////////////
	// Stamp structure
	//////////////////////////////////////////////////////////////////////////////////////////
	//! @brief 1区間の計測結果(名前と経過時間)を保持する構造体
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
		TimePointd<TimeUnit::millisecond> elapsed; //!< 累計経過時間
		TimePointd<TimeUnit::millisecond> section; //!< 区間時間

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


