#include "Particle.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Console/SystemConsole.h>
#include <Engine/Game/SxavengerGame.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Particle class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ParitcleCollection::Init() {
	BaseBehavior::renderingFlag_ = kBehaviorRender_Adaptive;

	Sxavenger::LoadTexture("resources/particleDemo.png");

	model_ = SxavengerGame::LoadModel("resources/model/CG2", "plane.obj");

	CreatePipeline();
	CreateBuffer();

	SetName("particle collection");
}

void ParitcleCollection::Term() {
}

void ParitcleCollection::Update() {

	instanceCount_ = 0;

	CineCamera* camera = sSystemConsole->GetGameCamera();

	for (auto element = elements_.begin(); element != elements_.end();) {

		// 秒数を超えていたら削除
		if (element->currentTime >= element->leftTime) {
			element = elements_.erase(element);
			continue;
		}

		element->currentTime.AddDeltaTime();

		element->transform.translate += element->velocity * Performance::GetDeltaTime(s).time;
		element->transform.rotate = ToQuaternion(camera->GetTransform().rotate) * MakeAxisAngle({ 0.0f, 1.0f, 0.0f }, pi_v * 2.0f);

		// bufferに登録
		if (instanceCount_ < kMaxInstanceCount_) {
			(*transformBuffer_)[instanceCount_].Transfer(element->transform.ToMatrix());
			(*infoBuffer_)[instanceCount_].color = element->color;
			instanceCount_++;
		}

		element++;
	}

}

void ParitcleCollection::DrawAdaptive(_MAYBE_UNUSED const Camera3D* camera) {

	auto commandList = Sxavenger::GetCommandList();

	pipeline_->SetPipeline(commandList);

	model_->SetIABuffer(0);

	commandList->SetGraphicsRootConstantBufferView(0, camera->GetGPUVirtualAddress());
	commandList->SetGraphicsRootShaderResourceView(1, transformBuffer_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootShaderResourceView(2, infoBuffer_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootDescriptorTable(3, Sxavenger::GetTextureHandleGPU("resources/particleDemo.png"));

	model_->DrawCall(0, instanceCount_);
}

void ParitcleCollection::SetAttributeImGui() {

	if (ImGui::Button("emit")) {
		MakeParticle();
	}
}

void ParitcleCollection::CreatePipeline() {

	blob_ = std::make_unique<GraphicsBlob>();
	blob_->Create(L"particle/particle.vs.hlsl", GRAPHICS_VERTEX);
	blob_->Create(L"particle/particle.ps.hlsl", GRAPHICS_PIXEL);

	pipeline_ = std::make_unique<GraphicsPipeline>();

	GraphicsRootSignatureDesc rootDesc;
	rootDesc.SetCBV(0, VISIBILITY_VERTEX, 0);        //!< camera
	rootDesc.SetVirtualSRV(1, VISIBILITY_VERTEX, 0); //!< transformation
	rootDesc.SetVirtualSRV(2, VISIBILITY_VERTEX, 1); //!< info
	rootDesc.SetSRV(3, VISIBILITY_PIXEL, 0);         //!< texture
	rootDesc.SetSampler(MODE_WRAP, VISIBILITY_PIXEL, 0);

	pipeline_->CreateRootSignature(Sxavenger::GetDevicesObj(), rootDesc);

	GraphicsPipelineDesc desc;
	desc.CreateDefaultDesc();

	desc.SetBlendMode(kBlendModeAdd);
	desc.SetDepthStencil(true, D3D12_DEPTH_WRITE_MASK_ZERO);

	pipeline_->CreatePipeline(Sxavenger::GetDevicesObj(), blob_.get(), desc);

}

void ParitcleCollection::CreateBuffer() {

	transformBuffer_
		= std::make_unique<BufferResource<TransformationMatrix>>(Sxavenger::GetDevicesObj(), kMaxInstanceCount_);

	infoBuffer_
		= std::make_unique<BufferResource<ParticleInfo>>(Sxavenger::GetDevicesObj(), kMaxInstanceCount_);

	for (uint32_t i = 0; i < kMaxInstanceCount_; ++i) {
		(*infoBuffer_)[i].color = { 1.0f, 1.0f, 1.0f, 1.0f };

		EulerTransform transform = {};
		transform.translate.x = i * 0.2f;
		transform.translate.y = i * 0.2f;

		(*transformBuffer_)[i].Transfer(transform.ToMatrix());
	}

}

void ParitcleCollection::MakeParticle() {

	ParticleElement element = {};
	element.Init();

	elements_.emplace_back(element);

}
