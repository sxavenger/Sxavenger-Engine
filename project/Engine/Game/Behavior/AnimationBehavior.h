#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include "ModelBehavior.h"

//* engine
#include <Engine/Game/Animation.h>
#include <Engine/System/Performance.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AnimationBehavior class
////////////////////////////////////////////////////////////////////////////////////////////
class AnimationBehavior
	: public ModelBehavior {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	AnimationBehavior() { Init(); }
	~AnimationBehavior() { Term(); }

	void Init();

	void Term();

	//* animation option *//

	void CreateAnimation();

	void UpdateAnimator(uint32_t animaitonIndex = 0, bool isLoop = false);

	//void CreateRaytracingRecorder()
	// TODO: raytracingへの対応

	//* derivative behaivor methods *//

	void SystemAttributeImGui() override;

	virtual void DrawSystematic(_MAYBE_UNUSED const Camera3D* camera) override;

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	std::unique_ptr<Animator> animator_;
	DeltaTimePoint animationTime_ = {};

};