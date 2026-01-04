#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Module/GameObject/GameObject.h>

////////////////////////////////////////////////////////////////////////////////////////////
// PerformanceActor class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief performance表示用のactor.
class PerformanceActor
	: public SXAVENGER_ENGINE GameObject {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	PerformanceActor();
	~PerformanceActor() override = default;

	void Update();

	void SetPosition(const Vector2f& position);

private:
};
