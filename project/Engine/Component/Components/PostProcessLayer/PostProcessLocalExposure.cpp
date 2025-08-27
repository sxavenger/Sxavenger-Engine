#include "PostProcessLocalExposure.h"
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

void PostProcessLocalExposure::Parameter::Init() {
	aperture     = 4.0f;
	shutterSpeed = 60.0f;
	iso          = 100.0f;
	compensation = 1.0f;
}

void PostProcessLocalExposure::Parameter::SetImGuiCommand() {
	SxImGui::DragFloat("aperture", &aperture, 0.01f, 0.0f, std::nullopt, "%.3ff");
	SxImGui::DragFloat("shutter speed [1/s]", &shutterSpeed, 0.01f, 0.0f);
	SxImGui::DragFloat("iso", &iso, 1.0f, 0.0f);
	SxImGui::DragFloat("compensation", &compensation, 0.01f, -15.0f, 15.0f);
}

////////////////////////////////////////////////////////////////////////////////////////////
// PostProcessLocalExposure class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PostProcessLocalExposure::Init() {
	parameter_ = std::make_unique<DimensionBuffer<Parameter>>();
	parameter_->Create(SxavengerSystem::GetDxDevice(), 1);
	parameter_->At(0).Init();

	name_ = "Local Exposure";
}

void PostProcessLocalExposure::Process(const DirectXQueueContext* context, const ProcessInfo& info) {
	auto process = info.buffer->GetProcessTextures();
	process->NextProcess();
	process->GetCurrentTexture()->TransitionBeginUnordered(context);

	auto core = FRenderCore::GetInstance()->GetProcess();

	core->SetPipeline(FRenderCoreProcess::ProcessType::LocalExposure, context);

	BindBufferDesc desc = {};
	// common
	desc.Set32bitConstants("Dimension", 2, &info.buffer->GetSize());
	desc.Set32bitConstants("Infomation", 1, &info.weight);

	//* textures
	desc.SetHandle("gInput",  process->GetPrevTexture()->GetGPUHandleSRV());
	desc.SetHandle("gOutput", process->GetCurrentTexture()->GetGPUHandleUAV());

	// parameter
	desc.SetAddress("gParameter", parameter_->GetGPUVirtualAddress());

	core->BindComputeBuffer(FRenderCoreProcess::ProcessType::LocalExposure, context, desc);
	core->Dispatch(context, info.buffer->GetSize());

	process->GetCurrentTexture()->TransitionEndUnordered(context);
}

void PostProcessLocalExposure::ShowInspectorImGui() {
	parameter_->At(0).SetImGuiCommand();
}
