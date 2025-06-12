#include "PostProcessLocalExposure.h"
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

void PostProcessLocalExposure::Process(const DirectXThreadContext* context, FRenderTargetTextures* textures, const CameraComponent* camera) {
	camera;

	auto process = textures->GetProcessTextures();
	process->NextProcess(context);

	auto core = FRenderCore::GetInstance()->GetProcess();

	core->SetPipeline(FRenderCoreProcess::ProcessType::LocalExposure, context);

	BindBufferDesc desc = {};
	// common
	desc.SetAddress("gConfig",    textures->GetDimension());
	desc.SetHandle("gInput",      textures->GetProcessTextures()->GetPrevTexture()->GetGPUHandleSRV());
	desc.SetHandle("gOutput",     textures->GetProcessTextures()->GetIndexTexture()->GetGPUHandleUAV());

	// parameter
	desc.SetAddress("gParameter", parameter_->GetGPUVirtualAddress());

	core->BindComputeBuffer(FRenderCoreProcess::ProcessType::LocalExposure, context, desc);
	core->Dispatch(context, textures->GetSize());
}

void PostProcessLocalExposure::ShowInspectorImGui() {
	parameter_->At(0).SetImGuiCommand();
}
