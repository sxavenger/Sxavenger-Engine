#include "FScene.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FScene class methods
////////////////////////////////////////////////////////////////////////////////////////////

AGeometryActor::Iterator FScene::AddGeometry(AGeometryActor* geometry) {
	return geometries_.emplace(geometries_.end(), geometry);
}

ALightActor::Iterator FScene::AddLight(ALightActor* light) {
	return lights_.emplace(lights_.end(), light);
}

void FScene::SetupTopLevelAS(const DirectXThreadContext* context) {
	topLevelAS_.BeginSetupInstance();

	AGeometryActor::SetupContext setupContext = {};
	setupContext.context    = context;
	setupContext.toplevelAS = &topLevelAS_;

	SetupTopLevelASContainer(setupContext, geometries_);

	topLevelAS_.EndSetupInstance(SxavengerSystem::GetDxDevice(), context->GetDxCommand());
	context->TransitionAllocator();
}

void FScene::SetupTopLevelASContainer(const AGeometryActor::SetupContext& context, const AGeometryActor::Container& container) {
	for (auto& geometry : container) {
		if (!geometry->IsActive()) {
			continue;
		}

		geometry->SetupToplevelAS(context);

		SetupTopLevelASContainer(context, geometry->GetChildren());
	}
}
