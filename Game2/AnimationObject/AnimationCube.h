#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// base
#include <AnimationObject.h>

// model
#include <Model.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AnimationCube class
////////////////////////////////////////////////////////////////////////////////////////////
class AnimationCube
	: public AnimationObject {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	AnimationCube() { Init(); }
	
	~AnimationCube() { Term(); }

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
	// FIXME: modelクラスに統合させること

	//* buffer *//
	std::unique_ptr<DxObject::BufferResource<Matrix4x4>> matrixBuffer_;

	EulerTransform worldTransform_;
	QuaternionTransform animationTransform_;

	//* member *//

	float animationTime_ = 0.0f;

};