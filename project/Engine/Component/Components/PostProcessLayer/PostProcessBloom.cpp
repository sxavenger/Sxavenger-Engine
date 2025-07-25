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
	intensity = 1.0f;
	threshold = 0.0f;
}

void PostProcessBloom::Parameter::SetImGuiCommand() {
	SxImGui::DragFloat("intensity", &intensity, 0.01f, 0.0f);
	SxImGui::DragFloat("threshold", &threshold, 0.01f, 0.0f);
}

////////////////////////////////////////////////////////////////////////////////////////////
// PostProcessBloom class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PostProcessBloom::Init() {
	parameter_ = std::make_unique<DimensionBuffer<Parameter>>();
	parameter_->Create(SxavengerSystem::GetDxDevice(), 1);
	parameter_->At(0).Init();

	name_ = "Bloom";
}

void PostProcessBloom::Process(const DirectXQueueContext* context, const ProcessInfo& info) {
	auto process = info.buffer->GetProcessTextures();
	process->NextProcess(context);

	auto core = FRenderCore::GetInstance()->GetProcess();

	core->SetPipeline(FRenderCoreProcess::ProcessType::Bloom, context);

	BindBufferDesc desc = {};
	// common
	desc.Set32bitConstants("Dimension", 2, &info.buffer->GetSize());

	//* texture
	desc.SetHandle("gInput",  process->GetPrevTexture()->GetGPUHandleSRV());
	desc.SetHandle("gOutput", process->GetIndexTexture()->GetGPUHandleUAV());

	// parameter
	desc.SetAddress("gParameter", parameter_->GetGPUVirtualAddress());

	core->BindComputeBuffer(FRenderCoreProcess::ProcessType::Bloom, context, desc);
	core->Dispatch(context, info.buffer->GetSize());
}

void PostProcessBloom::ShowInspectorImGui() {
	parameter_->At(0).SetImGuiCommand();
}
