#include "AnimationHuman.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// engine
#include <Performance.h>
#include <MyEngine.h>

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
	rootDesc.Resize(4, 1);
	rootDesc.SetCBV(0, VISIBILITY_VERTEX, 0);
	rootDesc.SetCBV(1, VISIBILITY_VERTEX, 1);
	rootDesc.SetVirtualSRV(2, VISIBILITY_VERTEX, 0);
	rootDesc.SetSRV(3, VISIBILITY_PIXEL, 0);
	rootDesc.SetSampler(0, MODE_WRAP, VISIBILITY_PIXEL, 0);

	pipeline_->CreateRootSignature(MyEngine::GetDevicesObj(), rootDesc);

	GraphicsPipelineDesc pipelineDesc;
	pipelineDesc.CreateDefaultDesc();

	// skinning追加分
	pipelineDesc.SetElement("WEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1);
	pipelineDesc.SetElement("INDEX",  0, DXGI_FORMAT_R32G32B32A32_SINT,  1);

	pipeline_->CreatePipeline(MyEngine::GetDevicesObj(), blob_.get(), pipelineDesc);

	/*
	 simpleSkin: "./Resources/model/simpleSkin", "simpleSkin.gltf"
	 human:      "./resources/model/human", "walk.gltf"
	*/

	model_ = std::make_unique<Model>("./resources/model/human", "walk.gltf");
	animation_ = ModelMethods::LoadAnimationFile("./resources/model/human", "walk.gltf");
	skeleton_ = ModelMethods::CreateSkeleton(model_->GetRootNode());
	skinCluster_ = ModelMethods::CreateSkinCluster(skeleton_, model_->GetModelData());
	
	matrixBuffer_ = std::make_unique<DxObject::BufferResource<Matrix4x4>>(MyEngine::GetDevicesObj(), 1);
	(*matrixBuffer_)[0] = Matrix4x4::Identity();

	SetThisAttribute("human");

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
	auto commandList = MyEngine::GetCommandList();

	pipeline_->SetPipeline(commandList);

	D3D12_VERTEX_BUFFER_VIEW vbvs[2] = {
		model_->GetMesh(0).vertexResource->GetVertexBufferView(),
		skinCluster_.influenceResource->GetVertexBufferView()
	};

	D3D12_INDEX_BUFFER_VIEW ibv = model_->GetMesh(0).indexResource->GetIndexBufferView();
	
	commandList->IASetVertexBuffers(0, _countof(vbvs), vbvs);
	commandList->IASetIndexBuffer(&ibv);

	commandList->SetGraphicsRootConstantBufferView(0, MyEngine::camera3D_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(1, matrixBuffer_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootShaderResourceView(2, skinCluster_.paletteResource->GetGPUVirtualAddress()); //!< dimentionBufferなのでvirtualSet
	model_->SetGraphicsTextureHandle(commandList, 0, 3);

	model_->DrawCall(commandList, 0);



	/*auto drawer = PrimitiveDrawer::GetInstance();

	const float radius = 0.2f;

	for (auto& joint : skeleton_.joints) {
		Vector3f pos = Matrix::Transform({0.0f, 0.0f, 0.0f}, joint.localMatrix);

		drawer->DrawSphere(pos, radius, 8, {0.0f, 1.0f, 0.0f, 1.0f});
	}

	drawer->DrawAll3D();*/

}

void AnimationHuman::SetAttributeImGui() {

	transform_.SetImGuiCommand();
	matrixBuffer_->operator[](0) = transform_.GetMatrix();

}
