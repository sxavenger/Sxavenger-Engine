#include "PostProcessLayerComponent.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* external
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// PostProcessLayerComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PostProcessLayerComponent::ShowComponentInspector() {

	ImGui::Text("tag");
	ImGui::Separator();

	static const char* kTags[] = {
		"None",
		"Local",
		"Global"
	};

	if (ImGui::BeginCombo("## tag", kTags[static_cast<uint8_t>(GetTag())])) {
		for (uint8_t i = 0; i < static_cast<uint8_t>(Tag::Global) + 1; ++i) {
			if (ImGui::Selectable(kTags[i], (i == static_cast<uint8_t>(GetTag())))) {
				SetTag(static_cast<Tag>(i));
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

void PostProcessLayerComponent::Process(const DirectXThreadContext* context, FRenderTargetTextures* textures, const CameraComponent* camera) {
	for (auto& process : processes_) {
		if (process->IsEnabled()) {
			process->Process(context, textures, camera);
		}
	}
}
