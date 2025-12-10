#include "EmitterComponent.h"
DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../Entity/MonoBehaviour.h"
#include "../Transform/TransformComponent.h"

//* engine
#include <Engine/System/SxavengerSystem.h>

//* lib
#include <Lib/Adapter/Random/Random.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sphere structure methods
////////////////////////////////////////////////////////////////////////////////////////////

EmitterComponent::BaseEmitter::Output EmitterComponent::Sphere::Emit() const {

	float r = Random::NormalDistributionRange(0.0f, radius);

	Vector2f angle = {
		Random::UniformDistribution(0.0f, kTau),
		Random::UniformDistribution(0.0f, kTau)
	};

	Output output = {};

	output.direction = {
		std::cos(angle.y) * std::cos(angle.x),
		std::sin(angle.y),
		std::cos(angle.y) * std::sin(angle.x)
	};

	output.position = output.direction * r;

	return output;

}

////////////////////////////////////////////////////////////////////////////////////////////
// EmitterComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

EmitterComponent::EmitterComponent(MonoBehaviour* behaviour) : BaseComponent(behaviour) {
}

void EmitterComponent::Update() {

	timer_.AddDeltaTime();

	if (timer_ < time_) {
		return; // 時間が経過していない場合は何もしない
	}

	timer_ = { TimePointf<TimeUnit::second>::Mod(timer_, time_).time };

	ParticleComponent* particle = GetParticleComponent();
	if (particle == nullptr) {
		return; //!< ParticleComponentが存在しない場合は何もしない
	}

	for (uint32_t i = 0; i < count_; ++i) {
		BaseEmitter::Output output = emitter_.Emit();
		output.position += RequireTransform()->GetPosition();

		particle->Emit(output.position, output.direction);
	}

}

const TransformComponent* EmitterComponent::RequireTransform() const {
	return BaseComponent::GetBehaviour()->RequireComponent<TransformComponent>();
}

ParticleComponent* EmitterComponent::GetParticleComponent() const {
	return BaseComponent::GetBehaviour()->GetComponent<ParticleComponent>();
}
