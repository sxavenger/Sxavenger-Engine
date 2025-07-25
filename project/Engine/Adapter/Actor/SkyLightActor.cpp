#include "SkyLightActor.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Asset/SxavengerAsset.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SkyLightActor class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SkyLightActor::Init(const Vector2f& size) {
	environment_.Create(size);
	skyLightComponent_ = AddComponent<SkyLightComponent>();

	SetName("sky light actor");
}

void SkyLightActor::Term() {
	environment_.Term();
}

void SkyLightActor::Update(bool isWait) {
	if (!MonoBehaviour::IsActive()) {
		return;
	}

	if (texture_.IsRegistered()) {
		environment_.SetEnvironment(texture_.WaitAcquire()->GetGPUHandleSRV());
	}

	environment_.Update(SxavengerSystem::GetDirectQueueContext());

	if (isWait) {
		environment_.WaitComplate();
		environment_.Update(SxavengerSystem::GetDirectQueueContext());
	}

	skyLightComponent_->SetEnvironment(environment_.GetMapEnvironment());
	skyLightComponent_->GetDiffuseParameter().SetTexture(environment_.UseIrradianceDescriptor(SxavengerSystem::GetDirectQueueContext()).GetIndex());
	skyLightComponent_->GetSpecularParameter().SetTexture(environment_.UseRadianceDescriptor(SxavengerSystem::GetDirectQueueContext()).GetIndex(), environment_.GetRadianceMiplevels());
}

void SkyLightActor::Inspectable() {
}

void SkyLightActor::SetTexture(const std::filesystem::path& filepath) {
	texture_ = SxavengerAsset::TryImport<AssetTexture>(filepath, Texture::Option{ Texture::Encoding::Intensity, false });
}

