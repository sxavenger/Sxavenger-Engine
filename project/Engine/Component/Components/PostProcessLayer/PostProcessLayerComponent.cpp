#include "PostProcessLayerComponent.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* external
#include <imgui.h>
#include <magic_enum.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// PostProcessLayerComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PostProcessLayerComponent::ShowComponentInspector() {

	ImGui::Text("tag");
	ImGui::Separator();

	if (ImGui::BeginCombo("## tag", magic_enum::enum_name(GetTag()).data())) {

		for (const auto& [value, name] : magic_enum::enum_entries<Tag>()) {
			if (ImGui::Selectable(name.data(), GetTag() == value)) {
				SetTag(value);
			}
		}

		ImGui::EndCombo();
	}

	ImGui::Text("post process layer");
	ImGui::Separator();

	for (auto& process : processes_) {
		ImGui::PushID(static_cast<void*>(process.get()));

		ImGui::Checkbox("## isEnabled", &process->IsEnabled());

		ImGui::SameLine();

		if (ImGui::CollapsingHeader(process->GetName().c_str())) {
			process->ShowInspectorImGui();
		}
		
		ImGui::PopID();
	}
}

void PostProcessLayerComponent::Process(const DirectXQueueContext* context, FRenderTargetTextures* textures, const CameraComponent* camera) {
	for (auto& process : processes_) {
		if (process->IsEnabled()) {
			process->Process(context, textures, camera);
		}
	}
}
