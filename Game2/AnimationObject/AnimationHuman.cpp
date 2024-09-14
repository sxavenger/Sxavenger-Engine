#include "AnimationHuman.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// engine
#include <Performance.h>
#include <Sxavenger.h>
#include <SxavengerGraphics.h>

// primitive
#include <PrimitiveDrawer.h>

//-----------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------
using namespace DxObject;

////////////////////////////////////////////////////////////////////////////////////////////
// AnimationHuman class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AnimationHuman::Init() {

	//* Graphics *//
	blob_ = std::make_unique<GraphicsBlob>();
	blob_->Create(L"animation/animation.VS.hlsl", GRAPHICS_VERTEX);
	blob_->Create(L"animation/animation.PS.hlsl", GRAPHICS_PIXEL);

	pipeline_ = std::make_unique<GraphicsPipeline>();

	GraphicsRootSignatureDesc rootDesc;
	rootDesc.SetCBV(0, VISIBILITY_ALL, 0);
	rootDesc.SetCBV(1, VISIBILITY_VERTEX, 1);
	rootDesc.SetVirtualSRV(2, VISIBILITY_VERTEX, 0);
	rootDesc.SetSRV(3, VISIBILITY_PIXEL, 0);
	rootDesc.SetSRV(4, VISIBILITY_PIXEL, 1);
	rootDesc.SetSampler(0, MODE_WRAP, VISIBILITY_PIXEL, 0);

	pipeline_->CreateRootSignature(Sxavenger::GetDevicesObj(), rootDesc);

	GraphicsPipelineDesc pipelineDesc;
	pipelineDesc.CreateDefaultDesc();

	// skinning追加分
	pipelineDesc.SetElement("WEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1);
	pipelineDesc.SetElement("INDEX",  0, DXGI_FORMAT_R32G32B32A32_SINT,  1);

	pipeline_->CreatePipeline(Sxavenger::GetDevicesObj(), blob_.get(), pipelineDesc);

	//* Compute *//
	csBlob_ = std::make_unique<DxObject::CSBlob>();
	csBlob_->Create(L"animation/skinning.CS.hlsl");

	csPipeline_ = std::make_unique<DxObject::CSPipeline>();

	CSRootSignatureDesc csDesc;
	csDesc.SetVirtualSRV(0, 0);
	csDesc.SetVirtualSRV(1, 1);
	csDesc.SetVirtualSRV(2, 2);
	csDesc.SetCBV(3, 0);
	csDesc.SetVirtualUAV(4, 0);

	csPipeline_->CreatePipeline(csDesc, csBlob_.get());

	/*
	 simpleSkin: "./Resources/model/simpleSkin", "simpleSkin.gltf"
	 human:      "./resources/model/human", "walk.gltf"
	*/

	model_ = std::make_unique<Model>("./resources/model/human", "walk.gltf");
	animation_ = ModelMethods::LoadAnimationFile("./resources/model/human", "walk.gltf");
	skeleton_ = ModelMethods::CreateSkeleton(model_->GetRootNode());
	skinCluster_ = ModelMethods::CreateSkinCluster(skeleton_, model_->GetModelData());

	skinnedBuffer_ = std::make_unique<DxObject::UnorderedBufferResource<VertexData>>(Sxavenger::GetDevicesObj(), (*skinCluster_.informationResource)[0]);
	
	matrixBuffer_ = std::make_unique<DxObject::BufferResource<Matrix4x4>>(Sxavenger::GetDevicesObj(), 1);
	(*matrixBuffer_)[0] = Matrix4x4::Identity();

	SetThisAttribute("human");

	// 反射用環境テクスチャ
	Sxavenger::LoadTexture("./resources/rostock_laage_airport_4k.dds");

}

void AnimationHuman::Term() {
}

void AnimationHuman::Update() {

	animationTime_ += Performance::GetDeltaTime(SecondsUnit::s);
	animationTime_ = std::fmod(animationTime_, animation_.duration);

	skeleton_.Update(animation_, animationTime_);
	skinCluster_.Update(skeleton_);

}

void AnimationHuman::Draw() {

	// commandListの取得
	auto commandList = Sxavenger::GetCommandList();

	//* Compute *//
	csPipeline_->SetPipeline();

	commandList->SetComputeRootShaderResourceView(0, skinCluster_.paletteResource->GetGPUVirtualAddress());
	commandList->SetComputeRootShaderResourceView(1, model_->GetMesh(0).GetVertexBuffer()->GetGPUVirtualAddress());
	commandList->SetComputeRootShaderResourceView(2, skinCluster_.influenceResource->GetGPUVirtualAddress());
	commandList->SetComputeRootConstantBufferView(3, skinCluster_.informationResource->GetGPUVirtualAddress());
	commandList->SetComputeRootUnorderedAccessView(4, skinnedBuffer_->GetGPUVirtualAddress());

	csPipeline_->Dispatch(RoundUp((*skinCluster_.informationResource)[0], 1024), 1, 1);

	// vertexとして使うためのbarrier
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource   = skinnedBuffer_->GetResource();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;

	commandList->ResourceBarrier(1, &barrier);

	//* Graphics *//
	pipeline_->SetPipeline(commandList);

	D3D12_VERTEX_BUFFER_VIEW vbv = skinnedBuffer_->GetVertexBufferView();
	D3D12_INDEX_BUFFER_VIEW ibv  = model_->GetMesh(0).GetIndexBuffer()->GetIndexBufferView();
	
	commandList->IASetVertexBuffers(0, 1, &vbv);
	commandList->IASetIndexBuffer(&ibv);

	commandList->SetGraphicsRootConstantBufferView(0, SxavengerGraphics::camera3D->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(1, matrixBuffer_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootShaderResourceView(2, skinCluster_.paletteResource->GetGPUVirtualAddress()); //!< dimentionBufferなのでvirtualSet
	model_->SetGraphicsTextureHandle(commandList, 0, 3);
	commandList->SetGraphicsRootDescriptorTable(4, Sxavenger::GetTextureHandleGPU("./resources/rostock_laage_airport_4k.dds"));

	model_->DrawCall(commandList, 0);

}

void AnimationHuman::SetAttributeImGui() {

	transform_.SetImGuiCommand();
	matrixBuffer_->operator[](0) = transform_.ToMatrix();

}
