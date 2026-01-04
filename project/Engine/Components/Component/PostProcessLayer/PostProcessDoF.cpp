#include "PostProcessDoF.h"
SXAVENGER_ENGINE_USING
DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/UI/SxImGui.h>
#include <Engine/System/System.h>
#include <Engine/Render/FRenderTargetBuffer.h>
#include <Engine/Render/FRenderCore.h>

//* external
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Parameter structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void PostProcessDoF::Parameter::Init() {
	focalDistance = 12.0f;

	isDebugView = false;
}

void PostProcessDoF::Parameter::SetImGuiCommand() {
	SxImGui::DragFloat("focal distance", &focalDistance, 0.01f, 0.0f);

	ImGui::CheckboxFlags("debug view", &isDebugView, true);
}


////////////////////////////////////////////////////////////////////////////////////////////
// PostProcessDoF class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PostProcessDoF::Init() {
	parameter_ = std::make_unique<ConstantBuffer<Parameter>>();
	parameter_->Create(System::GetDxDevice());
	parameter_->At().Init();

	name_ = "Depth of Field";
}

void PostProcessDoF::Process(const DirectXQueueContext* context, const ProcessInfo& info) {
	auto process = info.buffer->GetProcessTextures();
	process->NextProcess();
	process->GetCurrentTexture()->TransitionBeginUnordered(context);

	auto core = FRenderCore::GetInstance()->GetProcess();

	core->SetPipeline(FRenderCoreProcess::ProcessType::DoF, context);

	BindBufferDesc desc = {};
	//* deferred
	desc.SetHandle("gDepth", info.buffer->GetDepth()->GetRasterizerGPUHandleSRV());

	//* common
	desc.Set32bitConstants("Dimension", 2, &info.buffer->GetSize());

	//* textures
	desc.SetHandle("gInput",   process->GetPrevTexture()->GetGPUHandleSRV());
	desc.SetHandle("gOutput",  process->GetCurrentTexture()->GetGPUHandleUAV());

	// parameter
	desc.SetAddress("gCamera",    info.camera->GetGPUVirtualAddress());
	desc.SetAddress("gParameter", parameter_->GetGPUVirtualAddress());

	core->BindComputeBuffer(FRenderCoreProcess::ProcessType::DoF, context, desc);
	core->Dispatch(context, info.buffer->GetSize());

	process->GetCurrentTexture()->TransitionEndUnordered(context);
}

void PostProcessDoF::ShowInspectorImGui() {
	parameter_->At().SetImGuiCommand();
}
