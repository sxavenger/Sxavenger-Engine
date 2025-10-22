#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* preformace
#include "TimePoint.h"

//* engine
#include <Engine/System/SxavengerSystem.h>

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

	void AddDeltaTime();
	void SubtractionDeltaTime();

};

////////////////////////////////////////////////////////////////////////////////////////////
// DeltaTimePoint class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <TimeUnit _Unit, std::floating_point _Value>
inline void DeltaTimePoint<_Unit, _Value>::AddDeltaTime() {
	this->time += static_cast<_Value>(SxavengerSystem::GetDeltaTime().time);
}

template <TimeUnit _Unit, std::floating_point _Value>
inline void DeltaTimePoint<_Unit, _Value>::SubtractionDeltaTime() {
	this->time -= static_cast<_Value>(SxavengerSystem::GetDeltaTime().time);
}

////////////////////////////////////////////////////////////////////////////////////////////
// using
////////////////////////////////////////////////////////////////////////////////////////////

template <TimeUnit _Unit>
using DeltaTimePointf = DeltaTimePoint<_Unit, float>;

template <TimeUnit _Unit>
using DeltaTimePointd = DeltaTimePoint<_Unit, double>;
