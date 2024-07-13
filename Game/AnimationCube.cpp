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

	// Graphics
	blob_ = std::make_unique<GraphicsBlob>();
	blob_->Create(L"object3d/object3d.VS.hlsl", GRAPHICS_VERTEX);
	blob_->Create(L"object3d/object3d.PS.hlsl", GRAPHICS_PIXEL);

	pipeline_ = std::make_unique<GraphicsPipeline>();

	GraphicsRootSignatureDesc rootDesc;
	rootDesc.Resize(3, 1);
	rootDesc.SetCBV(0, VISIBILITY_VERTEX, 0);
	rootDesc.SetCBV(1, VISIBILITY_VERTEX, 1);
	rootDesc.SetSRV(2, VISIBILITY_PIXEL, 0);
	rootDesc.SetSampler(0, MODE_WRAP, VISIBILITY_PIXEL, 0);

	pipeline_->CreateRootSignature(MyEngine::GetDevicesObj(), rootDesc);

	GraphicsPipelineDesc pipelineDesc;
	pipelineDesc.CreateDefaultDesc();

	pipeline_->CreatePipeline(MyEngine::GetDevicesObj(), blob_.get(), pipelineDesc);
	
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
	NodeAnimation& rootNodeAnimation = animation_.nodeAnimations[model_->GetRootNode().name];
	
	animationTransform_.scale = AnimationMethods::CalculateValue(rootNodeAnimation.scale, animationTime_);
	animationTransform_.rotate = AnimationMethods::CalculateValue(rootNodeAnimation.rotate, animationTime_);
	animationTransform_.translate = AnimationMethods::CalculateValue(rootNodeAnimation.translate, animationTime_);
	
	// bufferに書き込み
	matrixBuffer_->operator[](0) = animationTransform_.GetMatrix() * worldTransform_.GetMatrix();

}

void AnimationCube::Draw() {

	auto commandList = MyEngine::GetCommandList();

	pipeline_->SetPipeline(commandList);

	model_->SetBuffers(commandList, 0);

	commandList->SetGraphicsRootConstantBufferView(0, MyEngine::camera3D_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(1, matrixBuffer_->GetGPUVirtualAddress());
	model_->SetGraphicsTextureHandle(commandList, 0, 2, TEXTURE_DIFFUSE);

	model_->DrawCall(commandList, 0);

}

void AnimationCube::SetAttributeImGui() {

	worldTransform_.SetImGuiCommand();

}
