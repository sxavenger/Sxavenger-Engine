#include "PostProcessTextureLUT.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Render/FRenderTargetTextures.h>
#include <Engine/Render/FRenderCore.h>

////////////////////////////////////////////////////////////////////////////////////////////
// PostProcessTextureLUT class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PostProcessTextureLUT::Init() {
	name_ = "LUT Texture";
}

void PostProcessTextureLUT::Process(const DirectXThreadContext* context, FRenderTargetTextures* textures, const CameraComponent* camera) {
	if (texture_ == nullptr) {
		return;
	}

	camera;

	auto process = textures->GetProcessTextures();
	process->NextProcess(context);

	auto core = FRenderCore::GetInstance()->GetProcess();

	core->SetPipeline(FRenderCoreProcess::ProcessType::TextureLUT, context);

	BindBufferDesc desc = {};
	// common
	desc.SetAddress("gConfig", textures->GetDimension());
	desc.SetHandle("gInput",   textures->GetProcessTextures()->GetPrevTexture()->GetGPUHandleSRV());
	desc.SetHandle("gOutput",  textures->GetProcessTextures()->GetIndexTexture()->GetGPUHandleUAV());

	// lut
	desc.SetHandle("gLUTTexture", texture_->GetGPUHandleSRV());

	core->BindComputeBuffer(FRenderCoreProcess::ProcessType::TextureLUT, context, desc);
	core->Dispatch(context, textures->GetSize());
}

void PostProcessTextureLUT::ShowInspectorImGui() {
	
}

void PostProcessTextureLUT::CreateTexture(const DirectXThreadContext* context, const AssetObserver<AssetTexture>& texture, const Vector2ui& tile) {
	texture_ = std::make_unique<FLUTTexture>();
	texture_->Create(texture, tile);
	texture_->Dispatch(context);
}
