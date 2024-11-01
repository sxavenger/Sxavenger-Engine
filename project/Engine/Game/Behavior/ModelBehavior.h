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
#include <Engine/Game/Material.h>

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

	virtual void CreateRaytracingRecorder();

	//* derivative behaivor methods *//

	virtual void SystemAttributeImGui() override;

	virtual void DrawSystematic(_MAYBE_UNUSED const Camera3D* camera) override;
	virtual void DrawAdaptive(_MAYBE_UNUSED const Camera3D* camera) override;
	virtual void DrawRaytracing(_MAYBE_UNUSED DxrObject::TopLevelAS* tlas) override;

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* IA
	Model* model_ = nullptr;

	//* Buffer
	QuaternionTransformBuffer transform_;
	PBRMaterialBuffer material_;
	UVTransformBuffer uvTransform_;

	//* raytracing
	std::vector<std::unique_ptr<DxrObject::BufferRecoreder>> recorders_;

};