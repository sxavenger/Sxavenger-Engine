#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include "BaseBehavior.h"

//* engine
#include <Engine/Module/TransformComponent/TransformComponent.h>

////////////////////////////////////////////////////////////////////////////////////////////
// TransformBehavior class
////////////////////////////////////////////////////////////////////////////////////////////
class TransformBehavior
	: public BaseBehavior, public TransformComponent {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	TransformBehavior()  = default;
	~TransformBehavior() = default;

	//* derivative behaivor methods *//
	//* ImGui command
	virtual void SystemAttributeImGui() override;

protected:
};
