#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Component/Entity/MonoBehaviour.h>
#include <Engine/Adapter/Parameter/SerializeParameter.h>
#include <Engine/Adapter/Parameter/SerializeGui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SkyLightActor class
////////////////////////////////////////////////////////////////////////////////////////////
class ParameterActor
	: public MonoBehaviour {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ParameterActor();
	~ParameterActor() = default;

	void Inspectable() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	SerializeParameter<float> param1_    = { "parameter1", 0.5f };
	SerializeParameter<Vector2f> param2_ = { "parameter2", { 1.0f, 2.0f } };
	SerializeParameter<Vector3i> param3_ = { "parameter3", { 4, 5, 6 } };

};
