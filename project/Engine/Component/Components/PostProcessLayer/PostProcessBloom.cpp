#include "PostProcessBloom.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Render/FRenderTargetBuffer.h>
#include <Engine/Render/FRenderCore.h>
#include <Engine/System/UI/SxImGui.h>

//* external
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Parameter structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void PostProcessBloom::Parameter::Init() {
	intensity = 0.5f;
	threshold = 0.0f;
	radius    = 3.0f;
}

void PostProcessBloom::Parameter::SetImGuiCommand() {
	SxImGui::DragFloat("intensity", &intensity, 0.01f, 0.0f);
	SxImGui::DragFloat("threshold", &threshold, 0.01f, 0.0f);
	SxImGui::DragFloat("radius",    &radius,    0.01f, 0.0f);
}

////////////////////////////////////////////////////////////////////////////////////////////
// PostProcessBloom class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PostProcessBloom::Init() {
	parameter_ = std::make_unique<ConstantBuffer<Parameter>>();
	parameter_->Create(SxavengerSystem::GetDxDevice());
	parameter_->At().Init();

	name_ = "Bloom";
}

void PostProcessBloom::Process(const DirectXQueueContext* context, const ProcessInfo& info) {
	auto core = FRenderCore::GetInstance()->GetProcess();

	auto process = info.buffer->GetProcessTextures();
	process->ReqiureCount(3);
	process->NextProcess(2);

	FProcessTexture* source    = process->GetPrevTexture(2);
	FProcessTexture* luminance = process->GetPrevTexture(1);
	FProcessTexture* output    = process->GetCurrentTexture();
	
	{ //!< luminanceの生成

		luminance->TransitionBeginUnordered(context);

		core->SetPipeline(FRenderCoreProcess::ProcessType::BloomLuminance, context);

		BindBufferDesc desc = {};
		desc.Set32bitConstants("Dimension", 2, &info.buffer->GetSize());
		desc.SetAddress("gParameter", parameter_->GetGPUVirtualAddress());
		desc.SetHandle("gInput",      source->GetGPUHandleSRV());
		desc.SetHandle("gOutput",     luminance->GetGPUHandleUAV());

		core->BindComputeBuffer(FRenderCoreProcess::ProcessType::BloomLuminance, context, desc);
		core->Dispatch(context, info.buffer->GetSize());

		luminance->TransitionEndUnordered(context);
	}

	{ //!< bloomの適用

		output->TransitionBeginUnordered(context);

		core->SetPipeline(FRenderCoreProcess::ProcessType::BloomApply, context);

		BindBufferDesc desc = {};
		desc.Set32bitConstants("Dimension", 2, &info.buffer->GetSize());
		desc.SetAddress("gParameter", parameter_->GetGPUVirtualAddress());
		desc.SetHandle("gInput",      source->GetGPUHandleSRV());
		desc.SetHandle("gLuminance",  luminance->GetGPUHandleSRV());
		desc.SetHandle("gOutput",     output->GetGPUHandleUAV());

		core->BindComputeBuffer(FRenderCoreProcess::ProcessType::BloomApply, context, desc);
		core->Dispatch(context, info.buffer->GetSize());

		output->TransitionEndUnordered(context);
	}

}

void PostProcessBloom::ShowInspectorImGui() {
	parameter_->At().SetImGuiCommand();
}
