#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// base
#include <AnimationObject.h>

// model
#include <Model.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AnimationHuman class
////////////////////////////////////////////////////////////////////////////////////////////
class AnimationHuman
	: public AnimationObject {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	AnimationHuman() { Init(); }

	~AnimationHuman() { Term(); }

	void Init();

	void Term();

	void Update();

	void Draw();

	void SetAttributeImGui() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================
	
	//* IA *//
	std::unique_ptr<Model> model_;

	Animation animation_;
	Skeleton  skeleton_;
	// FIXME: modelクラスに統合させること
	
	float animationTime_ = 0.0f;

	//* member *//
	EulerTransform transform_;

	//* debug *//

};
