#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* scene
#include <Engine/Adapter/Scene/BaseScene.h>

//* engine
#include <Engine/System/Runtime/Performance/DeltaTimePoint.h>
#include <Engine/Component/Entity/MonoBehaviour.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ClearScene class
////////////////////////////////////////////////////////////////////////////////////////////
class ClearScene
	: public BaseScene {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init() override;

	void Update() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	TimePointf<TimeUnit::second> time_       = { 2.0f };
	DeltaTimePointf<TimeUnit::second> timer_ = { 0.0f };

	std::unique_ptr<MonoBehaviour> fade_ = nullptr;

};
