#include "PostProcessPosterize.h"
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

PostProcessPosterize::Parameter PostProcessPosterize::Parameter::Default() {
	Parameter parameter;
	parameter.coordinateLevel    = 100.0f;
	parameter.coordinateMode     = Mode::Round;
	parameter.colorLevel         = 10.0f;
	parameter.colorMode          = Mode::Round;
	parameter.colorBrightnessMin = 0.0f;

	return parameter;
}

////////////////////////////////////////////////////////////////////////////////////////////
// PostProcessPosterize class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PostProcessPosterize::Init() {
	parameter_.Create(System::GetDxDevice());
	parameter_.At() = Parameter::Default();

	name_ = "Posterize";
}

void PostProcessPosterize::Process(const DirectXQueueContext* context, const ProcessInfo& info) {

	FProcessBuffer* process = info.buffer->GetProcess();

	auto core = FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreProcess>(); //!< RenderCoreの確保.
	core->SetPipeline(FRenderCoreProcess::PostProcess::Posterize, context);

	DxObject::BindBufferDesc desc = {};

	//!< 共通parameterの設定
	desc.Set32bitConstants("Dimension", 2,   &info.buffer->GetResolution());
	desc.Set32bitConstants("Information", 1, &info.weight);

	//!< parameterの設定
	desc.SetAddress("gParameter", parameter_.GetGPUVirtualAddress());

	{ //!< Posterize

		process->Next();
		process->GetCurrentTexture().TransitionUnorderedAccess(context);

		//!< Bufferの設定
		desc.SetHandle("gInput",   process->GetPreviousTexture().GetGPUHandleSRV());
		desc.SetHandle("gOutput",  process->GetCurrentTexture().GetGPUHandleUAV());

		core->BindComputeBuffer(FRenderCoreProcess::PostProcess::Posterize, context, desc);
		core->Dispatch(context, info.buffer->GetResolution());

		process->GetCurrentTexture().TransitionDefaultState(context);
	}

}

void PostProcessPosterize::ShowInspectorImGui() {

	auto& parameter = parameter_.At();

	SxGui::DragScalar<float>("coordinate level", &parameter.coordinateLevel, 0.1f, kEpsilon);
	SxGui::ComboEnum("coordinate mode", &parameter.coordinateMode);

	SxGui::DragScalar<float>("color level", &parameter.colorLevel, 0.1f, kEpsilon);
	SxGui::ComboEnum("color mode", &parameter.colorMode);

	SxGui::DragScalar<float>("color brightness min", &parameter.colorBrightnessMin, 0.1f, 0.0f);

}
