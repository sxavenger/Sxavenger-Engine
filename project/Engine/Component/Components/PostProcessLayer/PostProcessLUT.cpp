#include "PostProcessLUT.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include <Engine/Render/FRenderCore.h>
#include <Engine/Render/FRenderTargetBuffer.h>

//* engine
#include <Engine/System/UI/SxImGui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// PostProcessLUT class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PostProcessLUT::Init() {
	name_ = "LUT";

	parameter_ = std::make_unique<DimensionBuffer<Parameter>>();
	parameter_->Create(SxavengerSystem::GetDxDevice(), 1);
	parameter_->At(0).intensity = 1.0f;
}

void PostProcessLUT::Process(const DirectXQueueContext* context, const ProcessInfo& info) {
	if (texture_ == nullptr) {
		return; //!< textureが設定されていない場合は処理しない
	}

	auto process = info.buffer->GetProcessTextures();
	process->NextProcess();
	process->GetCurrentTexture()->TransitionBeginUnordered(context);

	auto core = FRenderCore::GetInstance()->GetProcess();

	core->SetPipeline(FRenderCoreProcess::ProcessType::LUT, context);

	BindBufferDesc desc = {};
	// common
	desc.Set32bitConstants("Dimension", 2, &info.buffer->GetSize());
	desc.Set32bitConstants("Infomation", 1, &info.weight);

	//* textures
	desc.SetHandle("gInput", process->GetPrevTexture()->GetGPUHandleSRV());
	desc.SetHandle("gOutput", process->GetCurrentTexture()->GetGPUHandleUAV());

	// lut
	desc.SetHandle("gLUTTexture", texture_->GetGPUHandleSRV());
	desc.SetAddress("gParameter", parameter_->GetGPUVirtualAddress());

	core->BindComputeBuffer(FRenderCoreProcess::ProcessType::LUT, context, desc);
	core->Dispatch(context, info.buffer->GetSize());

	process->GetCurrentTexture()->TransitionEndUnordered(context);
}

void PostProcessLUT::ShowInspectorImGui() {

	bool isEnable = (texture_ != nullptr);

	{ //!< parameter
		ImGui::BeginDisabled(!isEnable);

		ImGui::DragFloat("intensity", &parameter_->At(0).intensity, 0.01f, 0.0f, 1.0f);

		ImGui::EndDisabled();
	}
}

void PostProcessLUT::CreateTexture(const DirectXQueueContext* context, const UAssetParameter<UAssetTexture>& texture, const Vector2ui& tile) {

	// 引数の保存
	referenceTexture_ = texture;
	tile_             = tile;

	texture_ = std::make_unique<FLUTTexture>();
	texture_->Create(referenceTexture_.WaitRequire(), tile_);
	texture_->Dispatch(context);
}
