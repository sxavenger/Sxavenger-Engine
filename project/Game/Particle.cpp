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

	emitter_.count = 4;
	emitter_.freqency = { 1.0f };

	field_.acceleration = { 2.0f, 0.0f, 0.0f };
	field_.min = { -2.0f, -2.0f, -2.0f };
	field_.max = { 2.0f, 2.0f, 2.0f };
}

void ParitcleCollection::Term() {
}

void ParitcleCollection::Update() {
	UpdateEmitter();
	UpdateParticle();
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
		Emit();
	}

	ImGui::DragFloat3("transform", &emitter_.transform.translate.x, 0.01f);
	ImGui::Checkbox("apply field", &isApplyField_);
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

void ParitcleCollection::UpdateParticle() {

	instanceCount_ = 0;

	CineCamera* camera = sSystemConsole->GetGameCamera();

	for (auto element = elements_.begin(); element != elements_.end();) {

		// 秒数を超えていたら削除
		if (element->currentTime >= element->lifeTime) {
			element = elements_.erase(element);
			continue;
		}

		element->currentTime.AddDeltaTime();

		if (isApplyField_) {
			if (field_.IsCollision(element->transform.translate)) {
				element->velocity += field_.acceleration * Performance::GetDeltaTime(s).time;
			}
		}

		element->transform.translate += element->velocity * Performance::GetDeltaTime(s).time;
		element->transform.rotate = ToQuaternion(camera->GetTransform().rotate) * MakeAxisAngle({ 0.0f, 1.0f, 0.0f }, pi_v * 2.0f);

		element->color.a = std::lerp(1.0f, 0.0f, element->currentTime.time / element->lifeTime.time);

		// bufferに登録
		if (instanceCount_ < kMaxInstanceCount_) {
			(*transformBuffer_)[instanceCount_].Transfer(element->transform.ToMatrix());
			(*infoBuffer_)[instanceCount_].color = element->color;
			instanceCount_++;
		}

		element++;
	}

}

void ParitcleCollection::UpdateEmitter() {

	emitter_.freqencyTimer.AddDeltaTime();

	if (emitter_.freqencyTimer >= emitter_.freqency) {
		emitter_.freqencyTimer.time = std::fmod(emitter_.freqencyTimer.time, emitter_.freqency.time);
		Emit();
	}
}

void ParitcleCollection::MakeParticle(const Vector3f& transform) {

	ParticleElement element = {};

	element.transform.translate = {
		Random::Generate(-0.8f, 0.8f),
		Random::Generate(-0.8f, 0.8f),
		Random::Generate(-0.2f, 0.2f),
	};

	element.transform.translate += transform;

	element.velocity = {
		Random::Generate(-0.2f, 0.2f),
		Random::Generate(-0.2f, 0.2f),
		Random::Generate(-0.2f, 0.2f),
	};

	element.lifeTime.time = Random::Generate(2.0f, 4.0f);

	element.color = {
		Random::Generate(0.0f, 1.0f),
		Random::Generate(0.0f, 1.0f),
		Random::Generate(0.0f, 1.0f),
		1.0f
	};

	elements_.emplace_back(element);
}

void ParitcleCollection::Emit() {
	for (uint32_t i = 0; i < emitter_.count; ++i) {
		MakeParticle(emitter_.transform.translate);
	}
}