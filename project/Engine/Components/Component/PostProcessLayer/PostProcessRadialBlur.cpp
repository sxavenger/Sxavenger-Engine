#include "PostProcessRadialBlur.h"
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

void PostProcessRadialBlur::Parameter::Init() {
	center       = { 0.0f, 0.0f };
	intensity    = 0.4f;
	quality      = 8;
	isStochastic = true;
}

void PostProcessRadialBlur::Parameter::SetImGuiCommand() {
	SxGui::DragVectorN<float, 2>("center", &center.x, 0.01f, -1.0f, 1.0f);
	SxGui::DragScalar<float>("intensity", &intensity, 0.01f, 0.0f, 10.0f);
	SxGui::DragScalar<uint32_t>("quality", &quality, 1.0f, 1, 16);
	ImGui::CheckboxFlags("stochastic", &isStochastic, 1);
}

////////////////////////////////////////////////////////////////////////////////////////////
// PostProcessRadialBlur class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PostProcessRadialBlur::Init() {
	parameter_ = std::make_unique<DxObject::ConstantBuffer<Parameter>>();
	parameter_->Create(System::GetDxDevice());
	parameter_->At().Init();

	name_ = "Radial Blur";
}

void PostProcessRadialBlur::Process(const DirectXQueueContext* context, const ProcessInfo& info) {

	FProcessBuffer* process = info.buffer->GetProcess();

	auto core = FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreProcess>(); //!< RenderCoreの確保.
	core->SetPipeline(FRenderCoreProcess::PostProcess::RadialBlur, context);

	{ //!< Radial Blur

		process->Next();
		process->GetCurrentTexture().TransitionUnorderedAccess(context);

		//!< parameterの設定
		DxObject::BindBufferDesc desc = {};

		//!< 共通parameterの設定
		desc.Set32bitConstants("Dimension", 2,   &info.buffer->GetResolution());
		desc.Set32bitConstants("Information", 1, &info.weight);

		//!< Bufferの設定
		desc.SetHandle("gInput",   process->GetPreviousTexture().GetGPUHandleSRV());
		desc.SetHandle("gOutput",  process->GetCurrentTexture().GetGPUHandleUAV());

		//!< parameterの設定
		desc.SetAddress("gParameter", parameter_->GetGPUVirtualAddress());

		core->BindComputeBuffer(FRenderCoreProcess::PostProcess::RadialBlur, context, desc);
		core->Dispatch(context, info.buffer->GetResolution());

		process->GetCurrentTexture().TransitionDefaultState(context);
	}
}

void PostProcessRadialBlur::ShowInspectorImGui() {
	parameter_->At().SetImGuiCommand();
}
