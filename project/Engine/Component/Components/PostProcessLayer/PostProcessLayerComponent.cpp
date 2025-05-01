#include "PostProcessLayerComponent.h"

////////////////////////////////////////////////////////////////////////////////////////////
// PostProcessLayerComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PostProcessLayerComponent::ShowComponentInspector() {
}

void PostProcessLayerComponent::Process(const DirectXThreadContext* context, FRenderTargetTextures* textures, const CameraComponent* camera) {
	for (auto& process : processes_) {
		if (process->IsEnabled()) {
			process->Process(context, textures, camera);
		}
	}
}
