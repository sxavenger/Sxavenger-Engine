#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include "BaseBehavior.h"

//* engine
#include <Engine/Game/Model.h>
#include <Engine/Game/Transform.h>

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

	//* derivative behaivor methods *//

	virtual void SystemAttributeImGui() override;
	virtual void DrawSystematic(_MAYBE_UNUSED const Camera3D* camera) override;

protected:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* IA
	Model* model_ = nullptr;

	//* Buffer
	QuaternionTransformBuffer transform_;

};