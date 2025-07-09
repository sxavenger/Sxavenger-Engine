#include "ParticleComponent.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Content/SxavengerContent.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Albedo structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void ParticleComponent::Albedo::Init() {
	type   = Type::Value;
	albedo = kWhite3<float>;
	index  = NULL;
}

void ParticleComponent::Albedo::SetValue(const Color3f& _albedo) {
	type   = Type::Value;
	albedo = _albedo;
}

void ParticleComponent::Albedo::SetTexture(uint32_t _index, const Color3f& _albedo) {
	type   = Type::Texture;
	index  = _index;
	albedo = _albedo;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Transparent structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void ParticleComponent::Transparent::Init() {
	type        = Type::Value;
	transparent = 1.0f;
	index       = NULL;
}

void ParticleComponent::Transparent::SetValue(float _transparent) {
	type        = Type::Value;
	transparent = _transparent;
}

void ParticleComponent::Transparent::SetTexture(uint32_t _index, float _transparent) {
	type        = Type::Texture;
	index       = _index;
	transparent = _transparent;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Particle sturcture methods
////////////////////////////////////////////////////////////////////////////////////////////

void ParticleComponent::Particle::Init() {
	albedo.Init();
	transparent.Init();

	isBillboard = true;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Element structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void ParticleComponent::Element::Init() {
	albedo.SetStart(kWhite3<float>);
	albedo.SetEnd(kWhite3<float>);

	transparent.SetStart(1.0f);
	transparent.SetEnd(1.0f);

	velocity.SetStart(kOrigin3<float>);
	velocity.SetEnd(kOrigin3<float>);
}

void ParticleComponent::Element::Update() {
	// time
	time.AddDeltaTime();
	const float t = time.time / lifeTime.time;

	// transform
	transform.translate += velocity.GetMotion(t);
	transform.scale += size.GetMotion(t);

	// color
	Color3f rgb = albedo.GetMotion(t);
	float   a   = transparent.GetMotion(t);
	color = {
		rgb.r, rgb.g, rgb.b, a
	};
}

bool ParticleComponent::Element::IsDelete() const {
	return time > lifeTime;
}

Matrix4x4 ParticleComponent::Element::GetMat() const {
	return transform.ToMatrix();
}

ParticleComponent::Particle ParticleComponent::Element::GetParticle() const {
	Particle p = {};
	p.Init();

	if (albedoIndex.has_value()) {
		p.albedo.SetTexture(albedoIndex.value(), { color.r, color.g, color.b });

	} else {
		p.albedo.SetValue({ color.r, color.g, color.b });
	}

	if (transparentIndex.has_value()) {
		p.transparent.SetTexture(transparentIndex.value(), color.a);

	} else {
		p.transparent.SetValue(color.a);
	}

	p.isBillboard = isBillboard;
	
	return p;
}

////////////////////////////////////////////////////////////////////////////////////////////
// ParticleComponent class
////////////////////////////////////////////////////////////////////////////////////////////

void ParticleComponent::Init(uint32_t count, BlendMode mode) {
	matrices_ = std::make_unique<DimensionBuffer<TransformationMatrix>>();
	matrices_->Create(SxavengerSystem::GetDxDevice(), count);

	particles_ = std::make_unique<DimensionBuffer<Particle>>();
	particles_->Create(SxavengerSystem::GetDxDevice(), count);

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
	desc.SetRTVFormat(DXGI_FORMAT_R32G32B32A32_FLOAT);
	desc.SetPrimitive(PrimitiveType::TrianglList);

	pipeline_->CreatePipeline(SxavengerSystem::GetDxDevice(), desc);
}

void ParticleComponent::Update() {

	for (auto itr = elements_.begin(); itr != elements_.end();) {
		itr->Update();

		if (itr->IsDelete()) {
			itr = elements_.erase(itr);
			continue;
		}

		itr++;
	}

	AssingBuffer();
}

void ParticleComponent::SetPrimitive(InputPrimitive&& primitive) {
	primitive_ = std::move(primitive);
}

ParticleComponent::Element& ParticleComponent::Emit(const Vector3f& position) {
	Element element = {};
	element.Init();
	element.transform.translate = position;

	return elements_.emplace_back(element);
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
	desc.SetAddress("gParticles",  particles_->GetGPUVirtualAddress());

	pipeline_->BindGraphicsBuffer(context->GetDxCommand(), desc);

	primitive_->DrawCall(context, instance_);
}

void ParticleComponent::AssingBuffer() {
	instance_ = 0;
	for (const auto& element : elements_ | std::views::reverse) {
		if (instance_ >= matrices_->GetSize()) {
			break;
		}

		matrices_->At(instance_).Transfer(element.GetMat());
		particles_->At(instance_) = element.GetParticle();

		instance_++;
	}
}
