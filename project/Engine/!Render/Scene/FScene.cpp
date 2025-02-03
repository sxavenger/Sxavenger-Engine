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

	SetupTopLevelASContainer(setupContext, geometries_);

	topLevelAS_.EndSetupInstance(SxavengerSystem::GetDxDevice(), context->GetDxCommand());
}

void FScene::SetupTopLevelASContainer(const AGeometryActor::SetupContext& context, const AGeometryActor::Container& container) {
	for (auto& geometry : container) {
		if (geometry->GetTransparency() == AGeometryActor::Transparency::Opaque) {
			geometry->SetupToplevelAS(context);
		}

		SetupTopLevelASContainer(context, geometry->GetChildren());
	}
}
