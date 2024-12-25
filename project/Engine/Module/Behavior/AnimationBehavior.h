#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include "ModelBehavior.h"

//* engine
//#include <Engine/Content/Animation/SkeletonMesh.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AnimationBehavior class
////////////////////////////////////////////////////////////////////////////////////////////
class AnimationBehavior
	: public ModelBehavior {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	AnimationBehavior()  = default;
	~AnimationBehavior() = default;

	//* derivative behaivor methods *//
	//* ImGui command

	virtual void SystemAttributeImGui() override;

	//* Draw methods

	virtual void DrawSystematic(_MAYBE_UNUSED const SxavGraphicsFrame* frame) override;
	virtual void DrawAdaptive(_MAYBE_UNUSED const SxavGraphicsFrame* frame) override;
	virtual void DrawLateAdaptive(_MAYBE_UNUSED const SxavGraphicsFrame* frame) override;

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//SkeletonMesh* skeletonMesh_ = nullptr;

};
