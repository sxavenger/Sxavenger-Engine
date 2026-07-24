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

PostProcessPostFx::Parameter PostProcessPostFx::Parameter::Default() {
	Parameter parameter = {};
	parameter.brightness   = 0.0f;
	parameter.saturation   = 0.0f;
	parameter.colorfulness = 0.0f;
	parameter.clarity      = 0.0f;
	parameter.lumaSharpen  = 0.0f;
	
	return parameter;
}

////////////////////////////////////////////////////////////////////////////////////////////
// PostProcessPostFx class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PostProcessPostFx::Init() {
	parameter_.Create(System::GetDxDevice());
	parameter_.At() = Parameter::Default();

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
	desc.SetAddress("gParameter", parameter_.GetGPUVirtualAddress());

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

	auto& parameter = parameter_.At();

	SxGui::SliderScalar<float>("brightness",   &parameter.brightness,   -1.0f, 1.0f);
	SxGui::SliderScalar<float>("saturation",   &parameter.saturation,   -1.0f, 1.0f);
	SxGui::SliderScalar<float>("colorfulness", &parameter.colorfulness, 0.0f, 1.0f);
	SxGui::SliderScalar<float>("clarity",      &parameter.clarity,      0.0f, 1.0f);
	SxGui::SliderScalar<float>("lumaSharpen",  &parameter.lumaSharpen,  0.0f, 1.0f);
}
