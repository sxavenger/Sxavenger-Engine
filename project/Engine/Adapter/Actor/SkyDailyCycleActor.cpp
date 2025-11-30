#include "SkyDailyCycleActor.h"

////////////////////////////////////////////////////////////////////////////////////////////
// SkyDailyCycleActor class methods
////////////////////////////////////////////////////////////////////////////////////////////

SkyDailyCycleActor::SkyDailyCycleActor() {
	MonoBehaviour::SetName("sky daily cycle actor");

	transform_     = MonoBehaviour::AddComponent<TransformComponent>();
	skyLight_      = MonoBehaviour::AddComponent<SkyLightComponent>();
	skyAtmosphere_ = MonoBehaviour::AddComponent<SkyAtmosphereComponent>();

}

void SkyDailyCycleActor::Update() {
	if (!MonoBehaviour::IsActive()) {
		return;
	}

	skyAtmosphere_->UpdateTransmittance(SxavengerSystem::GetDirectQueueContext());
	skyAtmosphere_->UpdateMultipleScattering(SxavengerSystem::GetDirectQueueContext());
	skyAtmosphere_->UpdateSkyCube(SxavengerSystem::GetDirectQueueContext());

	float angle = Saturate(Vector3f::Dot(transform_->GetDirection(), kUp3<float>));

	skyAtmosphere_->SetIntensity(dayIntensity_);
	skyLight_->SetIntensity(angle * nightIntensity_);

}

void SkyDailyCycleActor::SetNightEnvironmentTexture(const AssetParameter<AssetTexture>& texture) {
	skyLight_->SetEnvironment(texture);
}
