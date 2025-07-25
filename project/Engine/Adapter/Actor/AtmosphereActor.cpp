#include "AtmosphereActor.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////////////////
// AtomosphereActor class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AtmosphereActor::Init(const Vector2f& size) {
	atmosphere_.Create(size);
	skyLightComponent_ = AddComponent<SkyLightComponent>();

	SetName("atmosphere actor");
}

void AtmosphereActor::Term() {
	atmosphere_.Term();
}

void AtmosphereActor::Update() {
	if (MonoBehaviour::IsActive() && isUpdateAtmosphere_) {
		atmosphere_.Update(SxavengerSystem::GetDirectQueueContext());
		skyLightComponent_->SetEnvironment(atmosphere_.UseAtmosphereDescriptor(SxavengerSystem::GetDirectQueueContext()).GetGPUHandle());
		skyLightComponent_->GetDiffuseParameter().SetTexture(atmosphere_.UseIrradianceDescriptor(SxavengerSystem::GetDirectQueueContext()).GetIndex());
		skyLightComponent_->GetSpecularParameter().SetTexture(atmosphere_.UseRadianceDescriptor(SxavengerSystem::GetDirectQueueContext()).GetIndex(), atmosphere_.GetRadianceMiplevels());
	}
}

void AtmosphereActor::Inspectable() {
	ImGui::Text("atmosphere");
	ImGui::Separator();

	atmosphere_.SetImGuiCommand();
	ImGui::Checkbox("update atmosphere", &isUpdateAtmosphere_);
}
