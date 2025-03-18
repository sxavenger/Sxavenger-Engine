#include "FSceneRenderer.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Component/ComponentStorage.h>


////////////////////////////////////////////////////////////////////////////////////////////
// Config structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void FSceneRenderer::Config::Apply() {
	// cameraの設定
	if (camera == nullptr) {
		if (tag != CameraComponent::Tag::None) {
			// component storage から tag に一致する camera component を取得.
			sComponentStorage->ForEach<CameraComponent>([&](CameraComponent* component) {
				if (component->GetTag() == tag) {
					camera = component;
				}
			});
		}
	}
}

Sxl::Flag<FSceneRenderer::Status> FSceneRenderer::Config::Check() const {
	return Sxl::Flag<Status>();
}
