#include "PostProcessAutoExposure.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/UI/SxImGui.h>
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Render/FRenderTargetBuffer.h>
#include <Engine/Render/FRenderCore.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Parameter structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void PostProcessAutoExposure::Parameter::Init() {
	minLogLuminance = 0.03f;
	maxLogLuminance = 20.0f;
	timeCoeff       = 0.1f;
	compensation    = 0.0f;
}

void PostProcessAutoExposure::Parameter::SetImGuiCommand() {
	ImGui::DragFloat("min log luminance", &minLogLuminance, 0.01f);
	ImGui::DragFloat("max log luminance", &maxLogLuminance, 0.01f);
	ImGui::SliderFloat("time coeff", &timeCoeff, 0.0f, 1.0f);
	ImGui::DragFloat("compensation", &compensation, 0.01f);
}

////////////////////////////////////////////////////////////////////////////////////////////
// PostProcessAutoExposure class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PostProcessAutoExposure::Init() {
	name_ = "Auto Exposure";

	parameter_ = std::make_unique<DimensionBuffer<Parameter>>();
	parameter_->Create(SxavengerSystem::GetDxDevice(), 1);
	parameter_->At(0).Init();

	histgram_ = std::make_unique<UnorderedDimensionBuffer<uint32_t>>();
	histgram_->Create(SxavengerSystem::GetDxDevice(), kGroupCount_);

	histgramShared_ = std::make_unique<UnorderedDimensionBuffer<uint32_t>>();
	histgramShared_->Create(SxavengerSystem::GetDxDevice(), kGroupCount_);

	averageLuminance_ = std::make_unique<UnorderedDimensionBuffer<float>>();
	averageLuminance_->Create(SxavengerSystem::GetDxDevice(), 1);

	debugHistgram_         = std::make_unique<ReadbackDimensionBuffer<uint32_t>>();
	debugAverageLuminance_ = std::make_unique<ReadbackDimensionBuffer<float>>();
}

void PostProcessAutoExposure::Process(const DirectXQueueContext* context, FRenderTargetBuffer* buffer, const CameraComponent* camera) {
	camera;

	auto process = buffer->GetProcessTextures();
	process->NextProcess(context);

	auto core = FRenderCore::GetInstance()->GetProcess();

	BindBufferDesc desc = {};
	desc.Set32bitConstants("Dimension", 2, &buffer->GetSize());

	// textures
	desc.SetHandle("gInput",   process->GetPrevTexture()->GetGPUHandleSRV());
	desc.SetHandle("gOutput",  process->GetIndexTexture()->GetGPUHandleUAV());

	// intermediate
	desc.SetAddress("gHistogram",        histgram_->GetGPUVirtualAddress());
	desc.SetAddress("gHistogramShared",  histgramShared_->GetGPUVirtualAddress());
	desc.SetAddress("gAverageLuminance", averageLuminance_->GetGPUVirtualAddress());

	// parameter
	desc.SetAddress("gParameter", parameter_->GetGPUVirtualAddress());

	core->SetPipeline(FRenderCoreProcess::ProcessType::AutoExposureLuminance, context);
	core->BindComputeBuffer(FRenderCoreProcess::ProcessType::AutoExposureLuminance, context, desc);
	core->Dispatch(context, buffer->GetSize());

	histgramShared_->Barrier(context->GetDxCommand());

	core->SetPipeline(FRenderCoreProcess::ProcessType::AutoExposureAverage, context);
	core->BindComputeBuffer(FRenderCoreProcess::ProcessType::AutoExposureAverage, context, desc);
	context->GetCommandList()->Dispatch(1, 1, 1);

	averageLuminance_->Barrier(context->GetDxCommand());

	core->SetPipeline(FRenderCoreProcess::ProcessType::AutoExposureApply, context);
	core->BindComputeBuffer(FRenderCoreProcess::ProcessType::AutoExposureApply, context, desc);
	core->Dispatch(context, buffer->GetSize());

}

void PostProcessAutoExposure::ShowInspectorImGui() {
	parameter_->At(0).SetImGuiCommand();

	ReadbackDimensionBuffer<uint32_t>::Readback(
		SxavengerSystem::GetDxDevice(),
		SxavengerSystem::GetDirectQueueContext()->GetDxCommand(),
		histgram_.get(),
		debugHistgram_.get()
	);

	ReadbackDimensionBuffer<float>::Readback(
		SxavengerSystem::GetDxDevice(),
		SxavengerSystem::GetDirectQueueContext()->GetDxCommand(),
		averageLuminance_.get(),
		debugAverageLuminance_.get()
	);

	auto itr = std::max_element(debugHistgram_->GetSpan().begin(), debugHistgram_->GetSpan().end());

	ImVec2 cursor = ImGui::GetCursorPos();
	ImVec2 size   = { ImGui::GetContentRegionAvail().x, 80.0f };

	SxImGui::PlotHistogramFunc(
		"## histogram",
		[&](uint32_t index) { return static_cast<float>(debugHistgram_->At(index)) / (*itr); },
		debugHistgram_->GetSize(),
		0,
		NULL,
		std::nullopt,
		std::nullopt,
		size
	);

	//float t = debugAverageLuminance_->At(0) / (parameter_->At(0).maxLogLuminance - parameter_->At(0).minLogLuminance);
	//ImGui::ProgressBar(t, { ImGui::GetContentRegionAvail().x, 0.0f }, "## luminance");

	
}

const PostProcessAutoExposure::Parameter& PostProcessAutoExposure::GetParameter() const {
	Exception::Assert(parameter_ != nullptr, "auto exposure parameter buffer is not create.");
	return parameter_->At(0);
}

PostProcessAutoExposure::Parameter& PostProcessAutoExposure::GetParameter() {
	Exception::Assert(parameter_ != nullptr, "auto exposure parameter buffer is not create.");
	return parameter_->At(0);
}
