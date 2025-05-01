#include "PostProcessExposure.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Render/FRenderTargetTextures.h>
#include <Engine/Render/FRenderCore.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Parameter structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void PostProcessExposure::Parameter::Init() {
	f            = 2.0f;
	shutterSpeed = 1.0f / 60.0f;
	iso          = 100.0f;
	compensation = 0.0f;
}

void PostProcessExposure::Parameter::SetImGuiCommand() {

}

////////////////////////////////////////////////////////////////////////////////////////////
// PostProcessExposure class
////////////////////////////////////////////////////////////////////////////////////////////

void PostProcessExposure::Init() {
	parameter_ = std::make_unique<DimensionBuffer<Parameter>>();
	parameter_->Create(SxavengerSystem::GetDxDevice(), 1);
	parameter_->At(0).Init();
}

void PostProcessExposure::Process(const DirectXThreadContext* context, FRenderTargetTextures* textures, const CameraComponent* camera) {
	auto process = textures->GetProcessTextures();
	process->NextProcess();

	auto core = FRenderCore::GetInstance()->GetProcess();

	core->SetPipeline(FRenderCoreProcess::ProcessType::Exposure, context);

	BindBufferDesc desc = {};
	desc.SetAddress("gConfig",    textures->GetDimension());
	desc.SetHandle("gInput",      textures->GetProcessTextures()->GetPrevTexture()->GetGPUHandleUAV());
	desc.SetHandle("gOutput",     textures->GetProcessTextures()->GetIndexTexture()->GetGPUHandleUAV());
	desc.SetAddress("gParameter", parameter_->GetGPUVirtualAddress());

	core->BindComputeBuffer(FRenderCoreProcess::ProcessType::Exposure, context, desc);
	core->Dispatch(context, textures->GetSize());

	camera;
}
