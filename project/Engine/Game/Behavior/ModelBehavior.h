#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include "BaseBehavior.h"

//* engine
#include <Engine/System/DxrObject/DxrBufferRecorder.h>
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

	void CreateRaytracingRecorder();

	//* derivative behaivor methods *//

	virtual void SystemAttributeImGui() override;
	virtual void DrawSystematic(_MAYBE_UNUSED const Camera3D* camera) override;
	virtual void DrawRaytracing(_MAYBE_UNUSED DxrObject::TopLevelAS* tlas) override;

protected:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* IA
	Model* model_ = nullptr;

	//* Buffer
	QuaternionTransformBuffer transform_;

	//* raytracing
	std::vector<std::unique_ptr<DxrObject::BufferRecoreder>> recorders_;

};