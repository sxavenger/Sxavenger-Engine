#include "PostProcessChromaticAberration.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include <Engine/System/System.h>
#include <Engine/Render/Buffer/FRenderTargetBuffer.h>
#include <Engine/Render/Core/FRenderCore.h>
#include <Engine/Render/Core/FRenderCoreProcess.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Parameter structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void PostProcessChromaticAberration::Parameter::Init() {
	intensity = 1.0f;
}

void PostProcessChromaticAberration::Parameter::SetImGuiCommand() {
	ImGui::DragFloat("intensity", &intensity, 0.01f, 0.0f, 5.0f);
}

////////////////////////////////////////////////////////////////////////////////////////////
// PostProcessChromaticAberration class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PostProcessChromaticAberration::Init() {
	parameter_ = std::make_unique<DxObject::ConstantBuffer<Parameter>>();
	parameter_->Create(System::GetDxDevice());
	parameter_->At().Init();

	name_ = "Chromatic Aberration";
}

void PostProcessChromaticAberration::Process(const DirectXQueueContext* context, const ProcessInfo& info) {

	FProcessBuffer* process = info.buffer->GetProcess();

	auto core = FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreProcess>(); //!< RenderCoreの確保.
	core->SetPipeline(FRenderCoreProcess::PostProcess::ChromaticAberration, context);

	{ //!< Chromatic Aberration

		process->Next();
		process->GetCurrentTexture().TransitionUnorderedAccess(context);

		//!< parameterの設定
		DxObject::BindBufferDesc desc = {};

		//!< 共通parameterの設定
		desc.Set32bitConstants("Dimension", 2,   &info.buffer->GetResolution());
		desc.Set32bitConstants("Information", 1, &info.weight);

		//!< Bufferの設定
		desc.SetHandle("gInput",  process->GetPreviousTexture().GetGPUHandleSRV());
		desc.SetHandle("gOutput", process->GetCurrentTexture().GetGPUHandleUAV());

		//!< parameterの設定
		desc.SetAddress("gParameter", parameter_->GetGPUVirtualAddress());

		core->BindComputeBuffer(FRenderCoreProcess::PostProcess::ChromaticAberration, context, desc);
		core->Dispatch(context, info.buffer->GetResolution());

		process->GetCurrentTexture().TransitionDefaultState(context);
	}
}

void PostProcessChromaticAberration::ShowInspectorImGui() {
	parameter_->At().SetImGuiCommand();
}
