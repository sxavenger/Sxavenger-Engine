#include "PostProcessDoF.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Render/FRenderTargetTextures.h>
#include <Engine/Render/FRenderCore.h>
#include <Engine/System/UI/SxImGui.h>

//* external
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Parameter structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void PostProcessDoF::Parameter::Init() {
	focalLength   = 12.0f;
	focalDistance = 12.0f;
	f             = 4.0f;
}

void PostProcessDoF::Parameter::SetImGuiCommand() {
	SxImGui::DragFloat("focal length",   &focalLength, 0.01f, 0.0f);
	SxImGui::DragFloat("focal distance", &focalDistance, 0.01f, 0.0f);
}


////////////////////////////////////////////////////////////////////////////////////////////
// PostProcessDoF class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PostProcessDoF::Init() {
	parameter_ = std::make_unique<DimensionBuffer<Parameter>>();
	parameter_->Create(SxavengerSystem::GetDxDevice(), 1);
	parameter_->At(0).Init();
}

void PostProcessDoF::Process(const DirectXThreadContext* context, FRenderTargetTextures* textures, const CameraComponent* camera) {
	auto process = textures->GetProcessTextures();
	process->NextProcess(context);

	auto core = FRenderCore::GetInstance()->GetProcess();

	core->SetPipeline(FRenderCoreProcess::ProcessType::DoF, context);

	BindBufferDesc desc = {};
	// deferred
	desc.SetHandle("gDepth", textures->GetDepth()->GetRasterizerGPUHandleSRV());

	// common
	desc.SetAddress("gConfig", textures->GetDimension());
	desc.SetHandle("gInput",   textures->GetProcessTextures()->GetPrevTexture()->GetGPUHandleSRV());
	desc.SetHandle("gOutput",  textures->GetProcessTextures()->GetIndexTexture()->GetGPUHandleUAV());

	// parameter
	desc.SetAddress("gCamera",    camera->GetGPUVirtualAddress());
	desc.SetAddress("gParameter", parameter_->GetGPUVirtualAddress());

	core->BindComputeBuffer(FRenderCoreProcess::ProcessType::DoF, context, desc);
	core->Dispatch(context, textures->GetSize());
}

void PostProcessDoF::ShowInspectorImGui() {

	ImGui::Checkbox("## isEnabled", &isEnabled_);

	ImGui::SameLine();

	if (ImGui::CollapsingHeader("DoF")) {
		parameter_->At(0).SetImGuiCommand();
	}
}
