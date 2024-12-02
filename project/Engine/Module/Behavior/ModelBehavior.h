#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include "BaseBehavior.h"

//* engine
#include <Engine/Content/Model/Model.h>
#include <Engine/Module/Transform/TransformComponent.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ModelBehavior class
////////////////////////////////////////////////////////////////////////////////////////////
class ModelBehavior
	: public BaseBehavior, public TransformComponent {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ModelBehavior()  { Init(); }
	~ModelBehavior() = default;

	void Init();

	//* derivative behaivor methods *//
	//* ImGui command
	virtual void SystemAttributeImGui() override;

	//* Draw methods

	virtual void DrawSystematic(_MAYBE_UNUSED const Camera3d* camera) override;
	virtual void DrawAdaptive(_MAYBE_UNUSED const Camera3d* camera) override;
	virtual void DrawLateAdaptive(_MAYBE_UNUSED const Camera3d* camera) override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Model* model_;

	std::unique_ptr<DxObject::DimensionBuffer<Matrix4x4>> mat_;
	//!< UVTransform(hack)

};