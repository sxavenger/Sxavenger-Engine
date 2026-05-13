#include "PostProcessPostFx.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/System.h>
#include <Engine/Render/Buffer/FRenderTargetBuffer.h>
#include <Engine/Render/Core/FRenderCore.h>
#include <Engine/Render/Core/FRenderCoreProcess.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Parameter structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void PostProcessPostFx::Parameter::Init() {
	brightness   = 0.0f;
	saturation   = 0.0f;
	colorfulness = 0.0f;
	clarity      = 0.0f;
	lumaSharpen  = 0.0f;
}

void PostProcessPostFx::Parameter::SetImGuiCommand() {
	SxGui::DragScalar<float>("brightness",   &brightness,   0.01f, -1.0f, 1.0f);
	SxGui::DragScalar<float>("saturation",   &saturation,   0.01f, -1.0f, 1.0f);
	SxGui::DragScalar<float>("colorfulness", &colorfulness, 0.01f, 0.0f, 1.0f);
	SxGui::DragScalar<float>("clarity",      &clarity,      0.01f, 0.0f, 1.0f);
	SxGui::DragScalar<float>("lumaSharpen",  &lumaSharpen,  0.01f, 0.0f, 1.0f);
}

////////////////////////////////////////////////////////////////////////////////////////////
// PostProcessPostFx class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PostProcessPostFx::Init() {
	parameter_ = std::make_unique<DxObject::ConstantBuffer<Parameter>>();
	parameter_->Create(System::GetDxDevice());
	parameter_->At().Init();

	name_ = "PostFx";
}

void PostProcessPostFx::Process(const DirectXQueueContext* context, const ProcessInfo& info) {

	FProcessBuffer* process = info.buffer->GetProcess();

	auto core = FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreProcess>(); //!< RenderCoreの確保.

	//!< parameterの設定
	DxObject::BindBufferDesc desc = {};

	//!< 共通parameterの設定
	desc.Set32bitConstants("Dimension", 2,   &info.buffer->GetResolution());
	desc.Set32bitConstants("Information", 1, &info.weight);

	//!< parameterの設定
	desc.SetAddress("gParameter", parameter_->GetGPUVirtualAddress());

	{ //!< Brightness

		core->SetPipeline(FRenderCoreProcess::PostProcess::PostFxBrightness, context);

		process->Next();
		process->GetCurrentTexture().TransitionUnorderedAccess(context);

		//!< Bufferの設定
		desc.SetHandle("gInput",   process->GetPreviousTexture().GetGPUHandleSRV());
		desc.SetHandle("gOutput",  process->GetCurrentTexture().GetGPUHandleUAV());

		core->BindComputeBuffer(FRenderCoreProcess::PostProcess::PostFxBrightness, context, desc);
		core->Dispatch(context, info.buffer->GetResolution());

		process->GetCurrentTexture().TransitionDefaultState(context);
	}

	{ //!< Saturation

		core->SetPipeline(FRenderCoreProcess::PostProcess::PostFxSaturation, context);

		process->Next();
		process->GetCurrentTexture().TransitionUnorderedAccess(context);

		//!< Bufferの設定
		desc.SetHandle("gInput", process->GetPreviousTexture().GetGPUHandleSRV());
		desc.SetHandle("gOutput", process->GetCurrentTexture().GetGPUHandleUAV());

		core->BindComputeBuffer(FRenderCoreProcess::PostProcess::PostFxSaturation, context, desc);

		core->Dispatch(context, info.buffer->GetResolution());
		process->GetCurrentTexture().TransitionDefaultState(context);
	}
	
	{ //!< Colorfulness

		core->SetPipeline(FRenderCoreProcess::PostProcess::PostFxColorfulness, context);

		process->Next();
		process->GetCurrentTexture().TransitionUnorderedAccess(context);

		//!< Bufferの設定
		desc.SetHandle("gInput", process->GetPreviousTexture().GetGPUHandleSRV());
		desc.SetHandle("gOutput", process->GetCurrentTexture().GetGPUHandleUAV());

		core->BindComputeBuffer(FRenderCoreProcess::PostProcess::PostFxColorfulness, context, desc);

		core->Dispatch(context, info.buffer->GetResolution());
		process->GetCurrentTexture().TransitionDefaultState(context);
	}
}

void PostProcessPostFx::ShowInspectorImGui() {
	parameter_->At().SetImGuiCommand();
}
