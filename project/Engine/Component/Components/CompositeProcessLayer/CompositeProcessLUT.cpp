#include "CompositeProcessLUT.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Render/FRenderCore.h>

////////////////////////////////////////////////////////////////////////////////////////////
// CompositeProcessLUT class methods
////////////////////////////////////////////////////////////////////////////////////////////

void CompositeProcessLUT::Init() {
	name_ = "LUT";
}

void CompositeProcessLUT::Process(const DirectXThreadContext* context, FRenderTargetTextures* textures) {
	if (texture_ == nullptr) {
		return;
	}

	auto core = FRenderCore::GetInstance()->GetProcess();

	core->SetPipeline(FRenderCoreProcess::CompositeType::LUT, context);

	

	BindBufferDesc desc = {};
	// common
	desc.SetAddress("gConfig", textures->GetDimension());
	desc.SetHandle("gTexture", textures->GetGBuffer(FRenderTargetTextures::GBufferLayout::Main)->GetGPUHandleUAV());

	// lut
	desc.SetHandle("gLUTTexture", texture_->GetGPUHandleSRV());

	core->BindComputeBuffer(FRenderCoreProcess::CompositeType::LUT, context, desc);
	core->Dispatch(context, textures->GetSize());
}

void CompositeProcessLUT::ShowInspectorImGui() {
	
}

void CompositeProcessLUT::CreateTexture(const DirectXThreadContext* context, const AssetObserver<AssetTexture>& texture, const Vector2ui& tile) {
	texture_ = std::make_unique<FLUTTexture>();
	texture_->Create(texture, tile);
	texture_->Dispatch(context);
}
