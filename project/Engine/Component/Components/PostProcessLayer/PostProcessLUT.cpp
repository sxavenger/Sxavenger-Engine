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

void PostProcessLUT::Process(const DirectXQueueContext* context, FRenderTargetBuffer* buffer, const CameraComponent* camera) {
	if (texture_ == nullptr) {
		return; //!< textureが設定されていない場合は処理しない
	}

	auto process = buffer->GetProcessTextures();
	process->NextProcess(context);

	auto core = FRenderCore::GetInstance()->GetProcess();

	core->SetPipeline(FRenderCoreProcess::ProcessType::LUT, context);

	BindBufferDesc desc = {};
	// common
	desc.Set32bitConstants("Dimension", 2, &buffer->GetSize());

	//* textures
	desc.SetHandle("gInput", process->GetPrevTexture()->GetGPUHandleSRV());
	desc.SetHandle("gOutput", process->GetIndexTexture()->GetGPUHandleUAV());

	// lut
	desc.SetHandle("gLUTTexture", texture_->GetGPUHandleSRV());
	desc.SetAddress("gParameter", parameter_->GetGPUVirtualAddress());

	core->BindComputeBuffer(FRenderCoreProcess::ProcessType::LUT, context, desc);
	core->Dispatch(context, buffer->GetSize());

	camera;
}

void PostProcessLUT::ShowInspectorImGui() {

	bool isEnable = (texture_ != nullptr);

	{ //!< parameter
		ImGui::BeginDisabled(!isEnable);

		ImGui::Text("parameter");
		ImGui::Separator();
		ImGui::DragFloat("intensity", &parameter_->At(0).intensity, 0.01f, 0.0f, 1.0f);

		ImGui::EndDisabled();
	}

	// todo: tileをなんやかんや...
}

void PostProcessLUT::CreateTexture(const DirectXQueueContext* context, const AssetObserver<AssetTexture>& texture, const Vector2ui& tile) {
	texture_ = std::make_unique<FLUTTexture>();
	texture_->Create(texture, tile);
	texture_->Dispatch(context);

	tile_ = tile;
}

void PostProcessLUT::CreateTexture(const DirectXQueueContext* context, const std::shared_ptr<AssetTexture>& texture, const Vector2ui& tile) {
	texture_ = std::make_unique<FLUTTexture>();
	texture_->Create(texture, tile);
	texture_->Dispatch(context);

	tile_ = tile;
}
