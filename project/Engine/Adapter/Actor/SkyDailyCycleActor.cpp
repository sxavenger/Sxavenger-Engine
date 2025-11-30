#include "SkyDailyCycleActor.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/UI/SxImGui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SkyDailyCycleActor class methods
////////////////////////////////////////////////////////////////////////////////////////////

SkyDailyCycleActor::SkyDailyCycleActor() {
	MonoBehaviour::SetName("sky daily cycle actor");

	transform_     = MonoBehaviour::AddComponent<TransformComponent>();
	skyLight_      = MonoBehaviour::AddComponent<SkyLightComponent>();
	skyAtmosphere_ = MonoBehaviour::AddComponent<SkyAtmosphereComponent>();
	MonoBehaviour::AddComponent<DirectionalLightComponent>();

	transform_->GetTransform().rotate = Quaternion::ToQuaternion({ kPi / 2.2f, 0.0f, 0.0f });
	//!< 初期回転を設定

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

void SkyDailyCycleActor::Inspectable() {
	SxImGui::DragScalar<float>("Day Intensity", &dayIntensity_,     0.1f, 0.0f);
	SxImGui::DragScalar<float>("Night Intensity", &nightIntensity_, 0.1f, 0.0f);
}

void SkyDailyCycleActor::SetNightEnvironmentTexture(const AssetParameter<AssetTexture>& texture) {
	skyLight_->SetEnvironment(texture);
}
