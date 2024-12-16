#include "Smoke.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Module/SxavengerGraphics/SxavGraphicsFrame.h>
#include <Engine/Module/Camera/Camera3d.h>
#include <Engine/Asset/SxavengerAsset.h>

//* lib
#include <Lib/Adapter/Random/Random.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Illmaination class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Smoke::Init() {
	SetName("smoke");

	//* buffer
	CreateInstance(kInstanceCount_);

	colorBuffer_ = std::make_unique<DimensionBuffer<Color4f>>();
	colorBuffer_->Create(SxavengerSystem::GetDxDevice(), kInstanceCount_);

	state_ = std::make_unique<ReflectionGraphicsPipelineState>();
	state_->CreateBlob("demo/smoke.vs.hlsl", GraphicsShaderType::vs);
	state_->CreateBlob("demo/smoke.ps.hlsl", GraphicsShaderType::ps);

	state_->ReflectionRootSignature(SxavengerSystem::GetDxDevice());

	GraphicsPipelineDesc desc = {};
	desc.CreateDefaultDesc();
	desc.SetBlendMode(0, BlendMode::kBlendModeAdd);
	desc.SetDepthStencil(true, D3D12_DEPTH_WRITE_MASK_ZERO);

	state_->CreatePipeline(SxavengerSystem::GetDxDevice(), desc);

	texture_ = SxavengerAsset::ImportTexture("asset/textures/particleDemo.png");
	//texture_ = SxavengerAsset::ImportTexture("asset/textures/tile_white.png");
	texture_->Load(SxavengerSystem::GetMainThreadContext());

	model_ = SxavengerAsset::ImportModel("asset/model/demo/plane.obj");
	model_->AsyncLoad();
	SxavengerSystem::PushTask(model_);
	model_->WaitCompleted();

	InstanceBehavior::SetRenderingFlag(BehaviorRenderingType::kAdaptive);
	InstanceBehavior::model_ = model_.get();
	
}

void Smoke::Term() {
}

void Smoke::Update() {

	elements_.remove_if([](Element& element) {
		element.currentTime.AddDeltaTime();
		return element.currentTime >= element.lifeTime;
	});

	for (auto& element : elements_) {

		float alpha = std::sin((element.currentTime.time / element.lifeTime.time) * pi_v * 2.0f);

		element.color = { 1.0f, 1.0f, 1.0f, alpha };

		element.transform.translate.y -= 2.0f * Performance::GetDeltaTime<TimeUnit::s>().time;
		element.transform.rotate *= ToQuaternion(element.addRotate);
	}

	instanceIndex_ = 0;

	for (const auto& element : elements_) {

		GetMatrix(instanceIndex_).Transfer(element.transform.ToMatrix());
		(*colorBuffer_)[instanceIndex_] = element.color;

		if (++instanceIndex_ >= kInstanceCount_) {
			break;
		}
	}
}

void Smoke::Emit(const Vector3f& position) {

	Element& element = elements_.emplace_back();

	element.transform.translate = position;
	element.transform.scale = { 0.4f, 0.4f, 0.4f };

	element.lifeTime.time = Random::Generate(4.0f, 6.0f);

	element.addRotate = { Random::Generate(-0.02f, 0.02f), Random::Generate(-0.02f, 0.02f), 0.0f };

}

void Smoke::SetAttributeImGui() {
}

void Smoke::DrawAdaptive(_MAYBE_UNUSED const SxavGraphicsFrame* frame) {

	if (model_ == nullptr) { return; }

	auto command = SxavengerSystem::GetMainThreadContext()->GetDxCommand();

	state_->ReloadAndSetPipeline(command, frame->GetSize());

	DxObject::BindBufferDesc bind = {};
	bind.SetAddress("gCamera",      frame->GetCamera()->GetGPUVirtualAddress());
	bind.SetAddress("gTransform",   matrixInstance_->GetGPUVirtualAddress());
	bind.SetAddress("gUVTransform", MaterialComponent::GetTransformGPUVirtualAddress());
	bind.SetAddress("gColor",       colorBuffer_->GetGPUVirtualAddress());
	bind.SetHandle("gAlbedo",       texture_->GetGPUHandleSRV());

	state_->BindGraphicsBuffer(command, bind);

	for (uint32_t i = 0; i < model_->GetMeshSize(); ++i) {
		model_->SetIABuffer(i);
		model_->DrawCall(i, instanceIndex_);
	}



}
