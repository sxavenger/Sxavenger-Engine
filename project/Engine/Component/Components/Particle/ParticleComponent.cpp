#include "ParticleComponent.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Particle structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void ParticleComponent::Particle::Update() {
	// time
	time.AddDeltaTime();
	const float t = time.time / lifeTime.time;

	// transform
	transform.translate += velocity.GetMotion(t);
}

bool ParticleComponent::Particle::IsDelete() const {
	return time > lifeTime;
}

////////////////////////////////////////////////////////////////////////////////////////////
// ParticleComponent class
////////////////////////////////////////////////////////////////////////////////////////////

void ParticleComponent::Init(uint32_t count) {
	matrices_ = std::make_unique<DimensionBuffer<TransformationMatrix>>();
	matrices_->Create(SxavengerSystem::GetDxDevice(), count);

	pipeline_ = std::make_unique<ReflectionGraphicsPipelineState>();
	pipeline_->CreateBlob(kPackagesShaderDirectory / "render/geometry/primitive/Default.vs.hlsl", GraphicsShaderType::vs);
	pipeline_->CreateBlob(kPackagesShaderDirectory / "render/geometry/primitive/Forward.ps.hlsl", GraphicsShaderType::ps);
	pipeline_->ReflectionRootSignature(SxavengerSystem::GetDxDevice());

	GraphicsPipelineDesc desc = {};
	desc.SetElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	desc.SetElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
	desc.SetElement("NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT);
	desc.SetRasterizer(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID);
	desc.SetDepthStencil(true, D3D12_DEPTH_WRITE_MASK_ZERO);
	desc.SetBlendMode(0, BlendMode::kBlendModeNormal);
	desc.SetDSVFormat(kDefaultDepthFormat);
	desc.SetRTVFormat(DXGI_FORMAT_R32G32B32A32_FLOAT);
	desc.SetPrimitive(PrimitiveType::TrianglList);

	pipeline_->CreatePipeline(SxavengerSystem::GetDxDevice(), desc);
}

void ParticleComponent::Update() {

	for (auto itr = particles_.begin(); itr != particles_.end();) {
		itr->Update();

		if (itr->IsDelete()) {
			itr = particles_.erase(itr);
			continue;
		}

		itr++;
	}

	AssingBuffer();
}

void ParticleComponent::SetPrimitive(InputPrimitive&& primitive) {
	primitive_ = std::move(primitive);
}

ParticleComponent::Particle& ParticleComponent::Emit(const Vector3f& position) {
	Particle particle = {};
	particle.transform.translate = position;

	return particles_.emplace_back(particle);
}

void ParticleComponent::DrawParticle(const DirectXThreadContext* context, const CameraComponent* camera) {
	if (!primitive_.has_value()) {
		return;
	}

	pipeline_->SetPipeline(context->GetDxCommand());

	primitive_->BindIABuffer(context);

	BindBufferDesc desc = {};
	desc.SetAddress("gCamera",     camera->GetGPUVirtualAddress());
	desc.SetAddress("gTransforms", matrices_->GetGPUVirtualAddress());

	pipeline_->BindGraphicsBuffer(context->GetDxCommand(), desc);

	primitive_->DrawCall(context, instance_);
}

void ParticleComponent::AssingBuffer() {
	instance_ = 0;
	for (const auto& element : particles_) {
		if (instance_ >= matrices_->GetSize()) {
			break;
		}

		matrices_->At(instance_++).Transfer(element.transform.ToMatrix());
	}
}
