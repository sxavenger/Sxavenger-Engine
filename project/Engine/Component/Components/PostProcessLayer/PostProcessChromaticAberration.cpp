#include "PostProcessChromaticAberration.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include <Engine/Render/FRenderTargetBuffer.h>
#include <Engine/Render/FRenderCore.h>

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
	parameter_ = std::make_unique<DimensionBuffer<Parameter>>();
	parameter_->Create(SxavengerSystem::GetDxDevice(), 1);
	parameter_->At(0).Init();

	name_ = "Chromatic Aberration";
}

void PostProcessChromaticAberration::Process(const DirectXQueueContext* context, const ProcessInfo& info) {

	auto process = info.buffer->GetProcessTextures();
	process->NextProcess(context);

	auto core = FRenderCore::GetInstance()->GetProcess();

	core->SetPipeline(FRenderCoreProcess::ProcessType::ChromaticAberration, context);

	BindBufferDesc desc = {};
	// common
	desc.Set32bitConstants("Dimension", 2, &info.buffer->GetSize());
	desc.Set32bitConstants("Infomation", 1, &info.weight);

	//* textures
	desc.SetHandle("gInput",  process->GetPrevTexture()->GetGPUHandleSRV());
	desc.SetHandle("gOutput", process->GetIndexTexture()->GetGPUHandleUAV());

	// parameter
	desc.SetAddress("gParameter", parameter_->GetGPUVirtualAddress());

	core->BindComputeBuffer(FRenderCoreProcess::ProcessType::ChromaticAberration, context, desc);
	core->Dispatch(context, info.buffer->GetSize());

}

void PostProcessChromaticAberration::ShowInspectorImGui() {
	parameter_->At(0).SetImGuiCommand();
}
