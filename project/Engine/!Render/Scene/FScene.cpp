#include "FScene.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FScene class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FScene::AddGeometry(AGeometryActor* geometry) {
	geometries_.emplace_back(geometry);
}

void FScene::AddLight(ALightActor* light) {
	lights_.emplace_back(light);
}

void FScene::SetupTopLevelAS(const DirectXThreadContext* context) {
	topLevelAS_.BeginSetupInstance();

	AGeometryActor::SetupContext setupContext = {};
	setupContext.context    = context;
	setupContext.toplevelAS = &topLevelAS_;

	for (auto& geometry : geometries_) {
		geometry->SetupToplevelAS(setupContext);
	}

	topLevelAS_.EndSetupInstance(SxavengerSystem::GetDxDevice(), context->GetDxCommand());
}
