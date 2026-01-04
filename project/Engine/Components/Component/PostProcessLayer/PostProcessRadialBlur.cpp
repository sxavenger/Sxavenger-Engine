#include "PostProcessRadialBlur.h"
SXAVENGER_ENGINE_USING
DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/System.h>
#include <Engine/Render/FRenderTargetBuffer.h>
#include <Engine/Render/FRenderCore.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Parameter structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void PostProcessRadialBlur::Parameter::Init() {
	center   = { 0.0f, 0.0f };
	intensity = 0.4f;
}

void PostProcessRadialBlur::Parameter::SetImGuiCommand() {
	ImGui::DragFloat2("center", &center.x, 0.01f, -1.0f, 1.0f);
	ImGui::DragFloat("intensity", &intensity, 0.01f, 0.0f, 10.0f);
}

////////////////////////////////////////////////////////////////////////////////////////////
// PostProcessRadialBlur class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PostProcessRadialBlur::Init() {
	parameter_ = std::make_unique<ConstantBuffer<Parameter>>();
	parameter_->Create(System::GetDxDevice());
	parameter_->At().Init();

	name_ = "Radial Blur";
}

void PostProcessRadialBlur::Process(const DirectXQueueContext* context, const ProcessInfo& info) {

	auto process = info.buffer->GetProcessTextures();
	process->NextProcess();
	process->GetCurrentTexture()->TransitionBeginUnordered(context);

	auto core = FRenderCore::GetInstance()->GetProcess();

	core->SetPipeline(FRenderCoreProcess::ProcessType::RadialBlur, context);

	BindBufferDesc desc = {};
	// common
	desc.Set32bitConstants("Dimension", 2, &info.buffer->GetSize());
	desc.Set32bitConstants("Infomation", 1, &info.weight);

	//* textures
	desc.SetHandle("gInput",  process->GetPrevTexture()->GetGPUHandleSRV());
	desc.SetHandle("gOutput", process->GetCurrentTexture()->GetGPUHandleUAV());

	// parameter
	desc.SetAddress("gParameter", parameter_->GetGPUVirtualAddress());

	core->BindComputeBuffer(FRenderCoreProcess::ProcessType::RadialBlur, context, desc);
	core->Dispatch(context, info.buffer->GetSize());

	process->GetCurrentTexture()->TransitionEndUnordered(context);
}

void PostProcessRadialBlur::ShowInspectorImGui() {
	parameter_->At().SetImGuiCommand();
}
