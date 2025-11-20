#include "SkyLightActor.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine

////////////////////////////////////////////////////////////////////////////////////////////
// SkyLightActor class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SkyLightActor::Init(const Vector2f& size) {
	environment_.Create(size);
	skyLightComponent_ = MonoBehaviour::AddComponent<SkyLightComponent>();

	MonoBehaviour::SetName("sky light actor");
}

void SkyLightActor::Term() {
	environment_.Term();
}

void SkyLightActor::Update(bool isWait) {
	if (!MonoBehaviour::IsActive()) {
		return;
	}

	if (texture_.Empty()) {
		return;
	}

	environment_.SetEnvironment(texture_.WaitRequire()->GetGPUHandleSRV());

	environment_.Update(SxavengerSystem::GetDirectQueueContext());

	if (isWait) {
		environment_.WaitComplate();
		environment_.Update(SxavengerSystem::GetDirectQueueContext());
	}

	skyLightComponent_->SetEnvironment(texture_.WaitRequire());
	skyLightComponent_->SetIrradiance(environment_.UseIrradianceDescriptor(SxavengerSystem::GetDirectQueueContext()));
	skyLightComponent_->SetRadiance(environment_.UseRadianceDescriptor(SxavengerSystem::GetDirectQueueContext()), environment_.GetRadianceMiplevels());
}

void SkyLightActor::Inspectable() {
}

void SkyLightActor::SetTexture(const AssetParameter<AssetTexture>& texture) {
	texture_ = texture;
}

