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
		skyLightComponent_->SetEnvironment(atmosphere_.UseAtmosphereDescriptor(SxavengerSystem::GetDirectQueueContext()));
		skyLightComponent_->SetIrradiance(atmosphere_.UseIrradianceDescriptor(SxavengerSystem::GetDirectQueueContext()));
		skyLightComponent_->SetRadiance(atmosphere_.UseRadianceDescriptor(SxavengerSystem::GetDirectQueueContext()), atmosphere_.GetRadianceMiplevels());
	}
}

void AtmosphereActor::Inspectable() {
	ImGui::Text("atmosphere");
	ImGui::Separator();

	atmosphere_.SetImGuiCommand();
	ImGui::Checkbox("update atmosphere", &isUpdateAtmosphere_);
}
