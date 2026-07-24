#include "PostProcessSketch.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include <Engine/System/System.h>
#include <Engine/Render/Buffer/FRenderTargetBuffer.h>
#include <Engine/Render/Buffer/FDepthStencilBuffer.h>
#include <Engine/Render/Buffer/FGBuffer.h>
#include <Engine/Render/Core/FRenderCore.h>
#include <Engine/Render/Core/FRenderCoreProcess.h>

//* external
#include <magic_enum.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// Parameter structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void PostProcessSketch::Parameter::Init() {
	noise_scale = { 1.0f, 1.0f };

	luminance_threshold_x = 0.5f;
	luminance_x_lit = 0.5f;

	luminance_threshold_y = 0.4f;
	luminance_y_lit = 0.2f;

	hatch_rotation_max = 0.0f;
	hatch_rotation_max = kPi;

	hatch_noise_threshold = 1.0f;
	hatch_noise_power = 1.0f;
	hatch_luminance_threshold = 0.5f;
}

void PostProcessSketch::Parameter::SetImGuiCommand() {
	SxGui::DragScalarN<float, 2>("noise_scale", &noise_scale.x, 0.01f);
	SxGui::DragScalar<float>("luminance_threshold_x", &luminance_threshold_x, 0.01f, 0.0f, std::nullopt);
	SxGui::DragScalar<float>("luminance_x_lit", &luminance_x_lit, 0.01f, 0.0f, std::nullopt);
	SxGui::DragScalar<float>("luminance_threshold_y", &luminance_threshold_y, 0.01f, 0.0f, std::nullopt);
	SxGui::DragScalar<float>("luminance_y_lit", &luminance_y_lit, 0.01f, 0.0f, std::nullopt);
	SxGui::DragScalar<float>("hatch_rotation_min", &hatch_rotation_min, 0.01f, 0.0f, std::nullopt);
	SxGui::DragScalar<float>("hatch_rotation_max", &hatch_rotation_max, 0.01f, 0.0f, std::nullopt);
	SxGui::DragScalar<float>("hatch_noise_threshold", &hatch_noise_threshold, 0.01f, 0.0f, std::nullopt);
	SxGui::DragScalar<float>("hatch_noise_power", &hatch_noise_power, 0.01f, 0.0f, std::nullopt);
	SxGui::DragScalar<float>("hatch_luminance_threshold", &hatch_luminance_threshold, 0.01f, 0.0f, std::nullopt);
}

////////////////////////////////////////////////////////////////////////////////////////////
// PostProcessSketch class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PostProcessSketch::Init() {
	parameter_ = std::make_unique<DxObject::ConstantBuffer<Parameter>>();
	parameter_->Create(System::GetDxDevice());
	parameter_->At().Init();

	name_ = "Sketch";
}

void PostProcessSketch::Process(const DirectXQueueContext* context, const ProcessInfo& info) {

	FProcessBuffer* process = info.buffer->GetProcess();

	if (noiseTexture_ == nullptr) {
		return; //!< noise textureが設定されていない場合は処理しない
	}

	auto core = FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreProcess>(); //!< RenderCoreの確保.
	core->SetPipeline(FRenderCoreProcess::PostProcess::Sketch, context);

	{ //!< Gray Scale

		process->Next();
		process->GetCurrentTexture().TransitionUnorderedAccess(context);

		//!< parameterの設定
		DxObject::BindBufferDesc desc = {};

		//!< 共通parameterの設定
		desc.Set32bitConstants("Dimension", 2, &info.buffer->GetResolution());
		desc.Set32bitConstants("Information", 1, &info.weight);
		desc.SetAddress("gCamera", info.camera->GetGPUVirtualAddress());

		//!< Bufferの設定
		desc.SetHandle("gInput",  process->GetPreviousTexture().GetGPUHandleSRV());
		desc.SetHandle("gOutput", process->GetCurrentTexture().GetGPUHandleUAV());

		//!< parameterの設定
		desc.SetAddress("gParameter", parameter_->GetGPUVirtualAddress());
		desc.SetHandle("gNoiseTexture", noiseTexture_.Require()->GetGPUHandleSRV());

		//!< GBufferの設定
		desc.SetHandle("gNormal", info.buffer->GetBuffer<FGBuffer>()->GetBuffer(FGBuffer::Layout::Normal).GetGPUHandleSRV());
		desc.SetHandle("gAlbedo", info.buffer->GetBuffer<FGBuffer>()->GetBuffer(FGBuffer::Layout::Albedo).GetGPUHandleSRV());
		desc.SetHandle("gDepth",  info.buffer->GetBuffer<FDepthStencilBuffer>()->GetBuffer(FDepthStencilBuffer::Layout::Scene).GetGPUHandleSRV());

		core->BindComputeBuffer(FRenderCoreProcess::PostProcess::Sketch, context, desc);
		core->Dispatch(context, info.buffer->GetResolution());

		process->GetCurrentTexture().TransitionDefaultState(context);
	}
}

void PostProcessSketch::ShowInspectorImGui() {
	parameter_->At().SetImGuiCommand();

	ImGui::Text("noise texture");

	sContentStorage->DragAndDropTargetContentFunc<ContentTexture>([this](const std::shared_ptr<ContentTexture>& content) {
		content->WaitComplete(); // contentの読み込みを待つ
		noiseTexture_ = content->GetId();
	});
}
