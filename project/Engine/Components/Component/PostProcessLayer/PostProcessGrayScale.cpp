#include "PostProcessGrayScale.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include <Engine/System/System.h>
#include <Engine/Render/Buffer/FRenderTargetBuffer.h>
#include <Engine/Render/Core/FRenderCore.h>
#include <Engine/Render/Core/FRenderCoreProcess.h>

//* external
#include <magic_enum.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// Parameter structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void PostProcessGrayScale::Parameter::Init() {
	type = Type::GrayScale;
}

void PostProcessGrayScale::Parameter::SetImGuiCommand() {
	SxGui::ComboEnum("Type", &type);
}

////////////////////////////////////////////////////////////////////////////////////////////
// PostProcessGrayScale class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PostProcessGrayScale::Init() {
	parameter_ = std::make_unique<DxObject::ConstantBuffer<Parameter>>();
	parameter_->Create(System::GetDxDevice());
	parameter_->At().Init();

	name_ = "Gray Scale";
}

void PostProcessGrayScale::Process(const DirectXQueueContext* context, const ProcessInfo& info) {

	FProcessBuffer* process = info.buffer->GetProcess();

	auto core = FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreProcess>(); //!< RenderCoreの確保.
	core->SetPipeline(FRenderCoreProcess::PostProcess::GrayScale, context);

	{ //!< Gray Scale

		process->Next();
		process->GetCurrentTexture().TransitionUnorderedAccess(context);

		//!< parameterの設定
		DxObject::BindBufferDesc desc = {};

		//!< 共通parameterの設定
		desc.Set32bitConstants("Dimension", 2, &info.buffer->GetResolution());
		desc.Set32bitConstants("Information", 1, &info.weight);

		//!< Bufferの設定
		desc.SetHandle("gInput",  process->GetPreviousTexture().GetGPUHandleSRV());
		desc.SetHandle("gOutput", process->GetCurrentTexture().GetGPUHandleUAV());

		//!< parameterの設定
		desc.SetAddress("gParameter", parameter_->GetGPUVirtualAddress());

		core->BindComputeBuffer(FRenderCoreProcess::PostProcess::GrayScale, context, desc);
		core->Dispatch(context, info.buffer->GetResolution());

		process->GetCurrentTexture().TransitionDefaultState(context);
	}
}

void PostProcessGrayScale::ShowInspectorImGui() {
	parameter_->At().SetImGuiCommand();
}
