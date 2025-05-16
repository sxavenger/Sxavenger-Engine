#include "PostProcessTextureLUT.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Render/FRenderTargetTextures.h>
#include <Engine/Render/FRenderCore.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Parameter structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void PostProcessTextureLUT::Parameter::Init() {
	tile = { 0, 0 };
}

////////////////////////////////////////////////////////////////////////////////////////////
// PostProcessTextureLUT class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PostProcessTextureLUT::Init() {
	parameter_ = std::make_unique<DimensionBuffer<Parameter>>();
	parameter_->Create(SxavengerSystem::GetDxDevice(), 1);
	parameter_->At(0).Init();

	name_ = "LUT Texture";
}

void PostProcessTextureLUT::Process(const DirectXThreadContext* context, FRenderTargetTextures* textures, const CameraComponent* camera) {
	if (!texture_.IsRegistered()) {
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

	// parameter
	desc.SetAddress("gParameter", parameter_->GetGPUVirtualAddress());
	desc.SetHandle("gLUTTexture", texture_.WaitAcquire()->GetGPUHandleSRV());

	core->BindComputeBuffer(FRenderCoreProcess::ProcessType::TextureLUT, context, desc);
	core->Dispatch(context, textures->GetSize());
}

void PostProcessTextureLUT::ShowInspectorImGui() {
	if (texture_.GetCondition() == decltype(texture_)::Condition::Valid) {
		auto texture = texture_.Get();

		if (texture->IsComplete()) {
			ImGui::Image(
				texture->GetGPUHandleSRV().ptr,
				ImVec2{ static_cast<float>(texture->GetSize().x), static_cast<float>(texture->GetSize().y) }
			);
		}
	}
}
