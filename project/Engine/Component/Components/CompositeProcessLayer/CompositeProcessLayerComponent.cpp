#include "CompositeProcessLayerComponent.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* external
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// CompositeProcessLayerComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void CompositeProcessLayerComponent::ShowComponentInspector() {
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

void CompositeProcessLayerComponent::Process(const DirectXQueueContext* context, FRenderTargetTextures* textures) {
	for (auto& process : processes_) {
		if (process->IsEnabled()) {
			process->Process(context, textures);
		}
	}
}
