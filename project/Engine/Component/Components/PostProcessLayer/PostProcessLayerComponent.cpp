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
	for (auto& process : processes_) {
		ImGui::PushID(static_cast<void*>(process.get()));
		process->ShowInspectorImGui();
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
