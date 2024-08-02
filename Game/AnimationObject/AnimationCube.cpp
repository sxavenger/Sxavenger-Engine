#include "AnimationCube.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// engine
#include <MyEngine.h>
#include <Performance.h>

//-----------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------
using namespace DxObject;

////////////////////////////////////////////////////////////////////////////////////////////
// AnimationCube class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AnimationCube::Init() {

	CreateGraphicsPipeline();
	
	// IA
	model_ = std::make_unique<Model>("./Resources/model/animatedCube", "animatedCube.gltf");
	animation_ = ModelMethods::LoadAnimationFile("./Resources/model/animatedCube", "animatedCube.gltf");

	// Buffer
	matrixBuffer_ = std::make_unique<BufferResource<Matrix4x4>>(MyEngine::GetDevicesObj(), 1);
	matrixBuffer_->operator[](0) = Matrix4x4::Identity();

	SetThisAttribute("animationCube");

}

void AnimationCube::Term() {
}

void AnimationCube::Update() {

	animationTime_ += Performance::GetDeltaTime(SecondsUnit::s); //!< deltaTimeから秒を取得
	animationTime_ = std::fmod(animationTime_, animation_.duration); //!< ループする

	// 参照取得
	const NodeAnimation& rootNodeAnimation = animation_.nodeAnimations[model_->GetRootNode().name];
	
	animationTransform_.scale = AnimationMethods::CalculateValue(rootNodeAnimation.scale, animationTime_);
	animationTransform_.rotate = AnimationMethods::CalculateValue(rootNodeAnimation.rotate, animationTime_);
	animationTransform_.translate = AnimationMethods::CalculateValue(rootNodeAnimation.translate, animationTime_);
	
	// bufferに書き込み
	matrixBuffer_->operator[](0) = animationTransform_.ToMatrix() * worldTransform_.ToMatrix();

}

void AnimationCube::Draw() {

	auto commandList = MyEngine::GetCommandList();

	pipeline_->SetPipeline(commandList);

	model_->SetBuffers(commandList, 0);

	commandList->SetGraphicsRootConstantBufferView(0, MyEngine::camera3D->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(1, matrixBuffer_->GetGPUVirtualAddress());
	model_->SetGraphicsTextureHandle(commandList, 0, 2, TEXTURE_DIFFUSE);

	model_->DrawCall(commandList, 0);

}

void AnimationCube::SetAttributeImGui() {

	worldTransform_.SetImGuiCommand();

}
