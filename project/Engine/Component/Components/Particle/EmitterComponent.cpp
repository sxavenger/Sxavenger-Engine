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
// Sphere structure methods
////////////////////////////////////////////////////////////////////////////////////////////

Vector3f EmitterComponent::Sphere::Emit() const {
	float r        = Random::NormalDistributionRange(0.0f, radius);
	Vector2f angle = {
		Random::UniformDistribution(0.0f, kTau),
		Random::UniformDistribution(0.0f, kTau)
	};

	return {
		r * std::cos(angle.y) * std::cos(angle.x),
		r * std::sin(angle.y),
		r * std::cos(angle.y) * std::sin(angle.x)
	};
}

////////////////////////////////////////////////////////////////////////////////////////////
// EmitterComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void EmitterComponent::Init() {
}

void EmitterComponent::Update() {

	timer_.AddDeltaTime();

	if (timer_ < time_) {
		return; // 時間が経過していない場合は何もしない
	}

	timer_ = { TimePointf<TimeUnit::second>::Mod(timer_, time_).time };


}

const TransformComponent* EmitterComponent::RequireTransform() const {
	return GetBehaviour()->RequireComponent<TransformComponent>();
}
