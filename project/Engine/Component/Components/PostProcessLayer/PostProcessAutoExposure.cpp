#include "PostProcessAutoExposure.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Render/FRenderTargetTextures.h>
#include <Engine/Render/FRenderCore.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Parameter structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void PostProcessAutoExposure::Parameter::Init() {
	minLogLuminance = 0.03f;
	maxLogLuminance = 8.0f;
	timeCoeff = 0.02f;
}

void PostProcessAutoExposure::Parameter::SetImGuiCommand() {
	ImGui::DragFloat("min log luminance", &minLogLuminance, 0.01f);
	ImGui::DragFloat("max log luminance", &maxLogLuminance, 0.01f);
	ImGui::SliderFloat("time coeff", &timeCoeff, 0.0f, 1.0f);
}

////////////////////////////////////////////////////////////////////////////////////////////
// PostProcessAutoExposure class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PostProcessAutoExposure::Init() {
	parameter_ = std::make_unique<DimensionBuffer<Parameter>>();
	parameter_->Create(SxavengerSystem::GetDxDevice(), 1);
	parameter_->At(0).Init();

	histgram_ = std::make_unique<UnorderedDimensionBuffer<uint32_t>>();
	histgram_->Create(SxavengerSystem::GetDxDevice(), kGroupCount_);

	histgramShared_ = std::make_unique<UnorderedDimensionBuffer<uint32_t>>();
	histgramShared_->Create(SxavengerSystem::GetDxDevice(), kGroupCount_);

	averageLuminance = std::make_unique<UnorderedDimensionBuffer<float>>();
	averageLuminance->Create(SxavengerSystem::GetDxDevice(), 1);

	name_ = "Auto Exposure";
}

void PostProcessAutoExposure::Process(const DirectXThreadContext* context, FRenderTargetTextures* textures, const CameraComponent* camera) {
	camera;

	auto process = textures->GetProcessTextures();
	process->NextProcess(context);

	auto core = FRenderCore::GetInstance()->GetProcess();

	BindBufferDesc desc = {};
	// textures
	desc.SetAddress("gConfig", textures->GetDimension());
	desc.SetHandle("gInput",   textures->GetProcessTextures()->GetPrevTexture()->GetGPUHandleSRV());
	desc.SetHandle("gOutput",  textures->GetProcessTextures()->GetIndexTexture()->GetGPUHandleUAV());

	// intermediate
	desc.SetAddress("gHistogram",        histgram_->GetGPUVirtualAddress());
	desc.SetAddress("gHistogramShared",  histgramShared_->GetGPUVirtualAddress());
	desc.SetAddress("gAverageLuminance", averageLuminance->GetGPUVirtualAddress());

	// parameter
	desc.SetAddress("gParameter", parameter_->GetGPUVirtualAddress());

	core->SetPipeline(FRenderCoreProcess::ProcessType::AutoExposureLuminance, context);
	core->BindComputeBuffer(FRenderCoreProcess::ProcessType::AutoExposureLuminance, context, desc);
	core->Dispatch(context, textures->GetSize());

	histgramShared_->Barrier(context->GetDxCommand());

	core->SetPipeline(FRenderCoreProcess::ProcessType::AutoExposureAverage, context);
	core->BindComputeBuffer(FRenderCoreProcess::ProcessType::AutoExposureAverage, context, desc);
	context->GetCommandList()->Dispatch(1, 1, 1);

	averageLuminance->Barrier(context->GetDxCommand());

	core->SetPipeline(FRenderCoreProcess::ProcessType::AutoExposureApply, context);
	core->BindComputeBuffer(FRenderCoreProcess::ProcessType::AutoExposureApply, context, desc);
	core->Dispatch(context, textures->GetSize());

}

void PostProcessAutoExposure::ShowInspectorImGui() {
	parameter_->At(0).SetImGuiCommand();
}
