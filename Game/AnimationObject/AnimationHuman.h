#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// base
#include <AnimationObject.h>

// DxObject
#include <DxCSBlob.h>
#include <DxCSPipelineState.h>
#include <DxCSBufferResource.h>

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

	//* Compute *//
	std::unique_ptr<DxObject::CSBlob>          csBlob_;
	std::unique_ptr<DxObject::CSPipelineState> csPipeline_;
	
	//* IA *//
	std::unique_ptr<Model> model_;

	Animation   animation_;
	Skeleton    skeleton_;
	SkinCluster skinCluster_;
	// FIXME: modelクラスに統合させること

	// uavBuffer
	std::unique_ptr<DxObject::CSBufferResource<VertexData>> skinnedBuffer_;

	//* member *//
	float animationTime_ = 0.0f;

	EulerTransform transform_;
	std::unique_ptr<DxObject::BufferResource<Matrix4x4>> matrixBuffer_;

	//* debug *//

};
