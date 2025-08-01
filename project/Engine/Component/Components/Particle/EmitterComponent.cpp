#include "EmitterComponent.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../Entity/MonoBehaviour.h"
#include "../Particle/GPUParticleComponent.h"

//* engine
#include <Engine/System/SxavengerSystem.h>

//* lib
#include <Lib/Adapter/Random/Random.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Emitter structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void EmitterComponent::Emitter::Init() {
	count  = 2;
	radius = 1.0f;
}

void EmitterComponent::Emitter::Seed() {
	seed = { Random::UniformDistribution<float>(0.0f, 1.0f), Random::UniformDistribution<float>(0.0f, 1.0f), Random::UniformDistribution<float>(0.0f, 1.0f) };
}

////////////////////////////////////////////////////////////////////////////////////////////
// EmitterComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void EmitterComponent::Init() {
	emitter_ = std::make_unique<DimensionBuffer<Emitter>>();
	emitter_->Create(SxavengerSystem::GetDxDevice(), 1);
	emitter_->At(0).Init();

	pipeline_ = std::make_unique<ReflectionComputePipelineState>();
	pipeline_->CreateBlob(kPackagesShaderDirectory / "render/Particle/GPUParticle/emitter.cs.hlsl");
	pipeline_->ReflectionPipeline(SxavengerSystem::GetDxDevice());
}

void EmitterComponent::Update(const DirectXQueueContext* context) {

	timer_.AddDeltaTime();

	if (timer_ < time_) {
		return; // 時間が経過していない場合は何もしない
	}

	timer_ = { TimePointf<TimeUnit::second>::Mod(timer_, time_).time };

	// emit処理
	EmitGPUParticle(context);

}

void EmitterComponent::EmitGPUParticle(const DirectXQueueContext* context) {

	auto component = GetBehaviour()->GetComponent<GPUParticleComponent>();

	if (component == nullptr) {
		return;
	}

	emitter_->At(0).Seed();

	// todo: pipelineの設定
	pipeline_->SetPipeline(context->GetDxCommand());

	BindBufferDesc desc = {};
	//* emitter
	desc.SetAddress("gEmitter",   emitter_->GetGPUVirtualAddress());
	desc.SetAddress("gTransform", RequireTransform()->GetGPUVirtualAddress());

	//* particle
	desc.SetAddress("gParticles", component->buffer_->GetGPUVirtualAddress());
	desc.Set32bitConstants("Dimension", 1, &component->buffer_->GetDimension());
	
	//* free list
	desc.SetHandle("gConsumeFreeIndex", component->freeList_->GetAppendCousumeGPUHandleUAV());
	desc.SetAddress("gCounter",         component->freeList_->GetCounterGPUVirtualAddress());

	pipeline_->BindComputeBuffer(context->GetDxCommand(), desc);
	pipeline_->Dispatch(context->GetDxCommand(), { 1, 1, 1 });
	
}

const TransformComponent* EmitterComponent::RequireTransform() const {
	return GetBehaviour()->RequireComponent<TransformComponent>();
}
