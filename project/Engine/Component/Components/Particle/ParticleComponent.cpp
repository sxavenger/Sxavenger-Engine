#include "ParticleComponent.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Render/GBuffer/FMainGBuffer.h>
#include <Engine/Preview/Content/ContentTexture.h>
#include <Engine/Preview/Content/ContentStorage.h>

//* externals
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Instance structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void ParticleComponent::Instance::Update() {
	timer.AddDeltaTime();

	if (timer >= parameter.time) {
		isDelete = true;
	}

	float t = std::clamp(timer.time / parameter.time.time, 0.0f, 1.0f);

	transform.scale = parameter.scale.Get(t);

	transform.translate += velocity * SxavengerSystem::GetDeltaTimef().time * parameter.velocity.Get(t);

	color       = parameter.color.Get(t);
	transparent = parameter.transparent.Get(t);
}

////////////////////////////////////////////////////////////////////////////////////////////
// ParticleComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

ParticleComponent::ParticleComponent(MonoBehaviour* behaviour) : BaseComponent(behaviour) {
	elements_ = std::make_unique<DxObject::DimensionBuffer<ParticleElement>>();
	matrices_ = std::make_unique<DxObject::DimensionBuffer<TransformationMatrix>>();

	albedoTexture_      = sContentStorage->Import<ContentTexture>("assets/textures/particleDemo.png")->GetId();
	transparentTexture_ = sContentStorage->Import<ContentTexture>("assets/textures/particleDemo.png")->GetId();
}

void ParticleComponent::ShowComponentInspector() {
	ImGui::Text(std::format("instance count: {}", instances_.size()).c_str());
	ImGui::Text(std::format("element count: {}",  elements_->GetSize()).c_str());
	ImGui::ProgressBar(static_cast<float>(elementCount_) / static_cast<float>(elements_->GetSize()), ImVec2(0.0f, 0.0f));
}

void ParticleComponent::Init(uint32_t elementCount, BlendMode mode) {
	elements_->Create(SxavengerSystem::GetDxDevice(), elementCount);
	matrices_->Create(SxavengerSystem::GetDxDevice(), elementCount);
	CreatePipeline(mode);
}

void ParticleComponent::Update() {
	UpdateInstance();
	UpdateBuffers();
}

void ParticleComponent::SetPrimitive(InputPrimitive&& primitive) {
	primitive_ = std::move(primitive);
}

void ParticleComponent::Emit(const Vector3f& position, const Vector3f& direction) {
	Instance& instance = instances_.emplace_back();
	instance.transform.translate = position;
	instance.velocity            = direction;
	instance.parameter           = function_();
}

void ParticleComponent::DrawParticle(const DirectXQueueContext* context, const CameraComponent* camera) {
	if (!primitive_.has_value() || elementCount_ == 0) {
		return;
	}

	pipeline_->SetPipeline(context->GetDxCommand());

	primitive_->BindIABuffer(context);

	BindBufferDesc desc = {};
	desc.SetAddress("gCamera",     camera->GetGPUVirtualAddress());
	desc.SetAddress("gTransforms", matrices_->GetGPUVirtualAddress());
	desc.SetAddress("gParticles",  elements_->GetGPUVirtualAddress());

	pipeline_->BindGraphicsBuffer(context->GetDxCommand(), desc);

	primitive_->DrawCall(context, elementCount_);
}

void ParticleComponent::CreatePipeline(BlendMode mode) {
	pipeline_ = std::make_unique<ReflectionGraphicsPipelineState>();
	pipeline_->CreateBlob(kPackagesShaderDirectory / "render/particle/particle.vs.hlsl", GraphicsShaderType::vs);
	pipeline_->CreateBlob(kPackagesShaderDirectory / "render/particle/particle.ps.hlsl", GraphicsShaderType::ps);
	pipeline_->ReflectionRootSignature(SxavengerSystem::GetDxDevice());

	GraphicsPipelineDesc desc = {};
	desc.SetElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	desc.SetElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
	desc.SetElement("NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT);
	desc.SetRasterizer(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID);
	desc.SetDepthStencil(true, D3D12_DEPTH_WRITE_MASK_ZERO);
	desc.SetBlendMode(0, mode);
	desc.SetDSVFormat(kDefaultDepthFormat);
	desc.SetRTVFormat(FMainGBuffer::kColorFormat);
	desc.SetPrimitive(PrimitiveType::TrianglList);

	pipeline_->CreatePipeline(SxavengerSystem::GetDxDevice(), desc);
}

void ParticleComponent::UpdateInstance() {
	for (auto itr = instances_.begin(); itr != instances_.end(); ) {
		itr->Update();

		if (itr->isDelete) { //!< 削除フラグが立っていたら削除
			itr = instances_.erase(itr);
			continue;
		}

		++itr;
	}
}

void ParticleComponent::UpdateBuffers() {

	elementCount_ = 0;

	for (const auto& instance : instances_) {
		if (elementCount_ >= elements_->GetSize()) {
			break;
		}

		ParticleElement& element = elements_->At(elementCount_);

		element.isBillboard = true;

		if (!albedoTexture_.Empty()) {
			auto texture = albedoTexture_.WaitRequire();
			element.albedo.SetTexture(texture->GetDescriptorSRV().GetIndex(), instance.color);

		} else {
			element.albedo.SetValue(instance.color);
		}

		if (!transparentTexture_.Empty()) {
			auto texture = transparentTexture_.WaitRequire();
			element.transparent.SetTexture(texture->GetDescriptorSRV().GetIndex(), instance.transparent);

		} else {
			element.transparent.SetValue(instance.transparent);
		}

		TransformationMatrix& matrix = matrices_->At(elementCount_);
		matrix.Transfer(instance.transform.ToMatrix());

		elementCount_++;
	}
}
