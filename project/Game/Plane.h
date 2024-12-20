#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include <Engine/Game/Behavior/ModelBehavior.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Plane class
////////////////////////////////////////////////////////////////////////////////////////////
class Plane
	: public ModelBehavior {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init();

	void SetAttributeImGui() override;

private:
};