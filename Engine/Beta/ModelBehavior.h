#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include "BaseBehavior.h"

//* engine
#include <Engine/Game/Transform.h>
#include <Engine/Game/Model.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ModelBehavior class
////////////////////////////////////////////////////////////////////////////////////////////
class ModelBehavior
	: public BaseBehavior {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ModelBehavior() { Init(); }

	~ModelBehavior() { Term(); }

	void Init();

	void Term();

	//* derivative behavior methods *//
	//* ImGui Commands
	virtual void SystemAttributeImGui() override;
	virtual void SetAttributeImGui() override {} //!< user define

	//* Draw
	virtual void SystemDraw() override;
	virtual void Draw() override {} //!< user define

	//* LocalMesh
	virtual void SystemDrawLocalMesh() override;

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* external *//

	Model*                    model_;
	QuaternionTransformBuffer transform_;

};