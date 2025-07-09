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
		atmosphere_.Update(SxavengerSystem::GetMainThreadContext());
		skyLightComponent_->SetEnvironment(atmosphere_.UseAtmosphereDescriptor(SxavengerSystem::GetMainThreadContext()).GetGPUHandle());
		skyLightComponent_->GetDiffuseParameter().SetTexture(atmosphere_.UseIrradianceDescriptor(SxavengerSystem::GetMainThreadContext()).GetIndex());
		skyLightComponent_->GetSpecularParameter().SetTexture(atmosphere_.UseRadianceDescriptor(SxavengerSystem::GetMainThreadContext()).GetIndex(), atmosphere_.GetRadianceMiplevels());
	}
}

void AtmosphereActor::Inspectable() {
	ImGui::Text("atmosphere");
	ImGui::Separator();

	atmosphere_.SetImGuiCommand();
	ImGui::Checkbox("update atmosphere", &isUpdateAtmosphere_);
}
