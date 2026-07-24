#include "PostProcessLocalExposure.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/UI/SxImGui.h>
#include <Engine/System/System.h>
#include <Engine/Render/Buffer/FRenderTargetBuffer.h>
#include <Engine/Render/Core/FRenderCore.h>
#include <Engine/Render/Core/FRenderCoreProcess.h>

//* external
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Parameter structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void PostProcessLocalExposure::Parameter::Init() {
	aperture     = 4.0f;
	shutterSpeed = 60.0f;
	iso          = 100.0f;
	compensation = 1.0f;
}

void PostProcessLocalExposure::Parameter::SetImGuiCommand() {
	SxImGui::DragFloat("aperture", &aperture, 0.01f, 0.0f, std::nullopt, "%.3ff");
	SxImGui::DragFloat("shutter speed [1/s]", &shutterSpeed, 0.01f, 0.0f);
	SxImGui::DragFloat("iso", &iso, 1.0f, 0.0f);
	SxImGui::DragFloat("compensation", &compensation, 0.01f, -15.0f, 15.0f);
}

////////////////////////////////////////////////////////////////////////////////////////////
// PostProcessLocalExposure class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PostProcessLocalExposure::Init() {
	parameter_ = std::make_unique<DxObject::ConstantBuffer<Parameter>>();
	parameter_->Create(System::GetDxDevice());
	parameter_->At().Init();

	name_ = "Local Exposure";
}

void PostProcessLocalExposure::Process(const DirectXQueueContext* context, const ProcessInfo& info) {

	FProcessBuffer* process = info.buffer->GetProcess();

	auto core = FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreProcess>(); //!< RenderCoreの確保.
	core->SetPipeline(FRenderCoreProcess::PostProcess::LocalExposure, context);

	{ //!< Local Exposure

		process->Next();
		process->GetCurrentTexture().TransitionUnorderedAccess(context);

		//!< parameterの設定
		DxObject::BindBufferDesc desc = {};

		//!< 共通parameterの設定
		desc.Set32bitConstants("Dimension", 2, &info.buffer->GetResolution());
		desc.Set32bitConstants("Information", 1, &info.weight);

		//!< Bufferの設定
		desc.SetHandle("gInput", process->GetPreviousTexture().GetGPUHandleSRV());
		desc.SetHandle("gOutput", process->GetCurrentTexture().GetGPUHandleUAV());

		//!< parameterの設定
		desc.SetAddress("gParameter", parameter_->GetGPUVirtualAddress());

		core->BindComputeBuffer(FRenderCoreProcess::PostProcess::LocalExposure, context, desc);
		core->Dispatch(context, info.buffer->GetResolution());

		process->GetCurrentTexture().TransitionDefaultState(context);
	}
}

void PostProcessLocalExposure::ShowInspectorImGui() {
	parameter_->At().SetImGuiCommand();
}
