#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Foundation.h>
#include <Engine/System/System.h>

//* lib
#include <Lib/Adapter/Time/TimePoint.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// DeltaTimePoint class
////////////////////////////////////////////////////////////////////////////////////////////
template <TimeUnit _Unit, std::floating_point _Value>
class DeltaTimePoint
	: public TimePoint<_Unit, _Value> {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	using TimePoint<_Unit, _Value>::TimePoint; //!< constructorの継承

	//* delta time option *//

	//! @brief DeltaTimeを加算する
	void AddDeltaTime();

	//! @brief DeltaTimeを減算する
	void SubtractionDeltaTime();

};

////////////////////////////////////////////////////////////////////////////////////////////
// DeltaTimePoint class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <TimeUnit _Unit, std::floating_point _Value>
inline void DeltaTimePoint<_Unit, _Value>::AddDeltaTime() {
	this->time += static_cast<_Value>(System::GetDeltaTimed().time);
}

template <TimeUnit _Unit, std::floating_point _Value>
inline void DeltaTimePoint<_Unit, _Value>::SubtractionDeltaTime() {
	this->time -= static_cast<_Value>(System::GetDeltaTimed().time);
}

////////////////////////////////////////////////////////////////////////////////////////////
// using
////////////////////////////////////////////////////////////////////////////////////////////

template <TimeUnit _Unit>
using DeltaTimePointf = DeltaTimePoint<_Unit, float>;

template <TimeUnit _Unit>
using DeltaTimePointd = DeltaTimePoint<_Unit, double>;

SXAVENGER_ENGINE_NAMESPACE_END
