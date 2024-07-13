#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// DxObject
#include <DxGraphicsBlob.h>
#include <DxGraphicsPipeline.h>
#include <DxBufferResource.h>

// c++
#include <memory>

// model
#include <Model.h>

// attribute
#include <Attribute.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AnimationCube class
////////////////////////////////////////////////////////////////////////////////////////////
class AnimationCube
	: public Attribute {
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

	//* Graphics *//
	std::unique_ptr<DxObject::GraphicsBlob>     blob_;
	std::unique_ptr<DxObject::GraphicsPipeline> pipeline_;

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