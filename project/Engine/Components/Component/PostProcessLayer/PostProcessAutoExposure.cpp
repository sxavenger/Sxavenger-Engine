#include "PostProcessAutoExposure.h"
SXAVENGER_ENGINE_USING
DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/StreamLogger.h>
#include <Engine/System/UI/SxImGui.h>
#include <Engine/System/System.h>
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

	parameter_ = std::make_unique<ConstantBuffer<Parameter>>();
	parameter_->Create(System::GetDxDevice());
	parameter_->At().Init();

	histgram_ = std::make_unique<UnorderedDimensionBuffer<uint32_t>>();
	histgram_->Create(System::GetDxDevice(), kGroupCount_);

	histgramShared_ = std::make_unique<UnorderedDimensionBuffer<uint32_t>>();
	histgramShared_->Create(System::GetDxDevice(), kGroupCount_);

	averageLuminance_ = std::make_unique<UnorderedDimensionBuffer<float>>();
	averageLuminance_->Create(System::GetDxDevice(), 1);

	debugHistgram_         = std::make_unique<ReadbackDimensionBuffer<uint32_t>>();
	debugAverageLuminance_ = std::make_unique<ReadbackDimensionBuffer<float>>();
}

void PostProcessAutoExposure::Process(const DirectXQueueContext* context, const ProcessInfo& info) {

	auto core = FRenderCore::GetInstance()->GetProcess();

	auto process = info.buffer->GetProcessTextures();
	process->NextProcess();
	process->GetCurrentTexture()->TransitionBeginUnordered(context);

	BindBufferDesc desc = {};
	// common
	desc.Set32bitConstants("Dimension", 2,  &info.buffer->GetSize());
	desc.Set32bitConstants("Information", 1, &info.weight);

	// textures
	desc.SetHandle("gInput",   process->GetPrevTexture()->GetGPUHandleSRV());
	desc.SetHandle("gOutput",  process->GetCurrentTexture()->GetGPUHandleUAV());

	// intermediate
	desc.SetAddress("gHistogram",        histgram_->GetGPUVirtualAddress());
	desc.SetAddress("gHistogramShared",  histgramShared_->GetGPUVirtualAddress());
	desc.SetAddress("gAverageLuminance", averageLuminance_->GetGPUVirtualAddress());

	// parameter
	desc.SetAddress("gParameter", parameter_->GetGPUVirtualAddress());

	{
		core->SetPipeline(FRenderCoreProcess::ProcessType::AutoExposureLuminance, context);
		core->BindComputeBuffer(FRenderCoreProcess::ProcessType::AutoExposureLuminance, context, desc);
		core->Dispatch(context, info.buffer->GetSize());

		histgramShared_->Barrier(context->GetDxCommand());
	}

	{
		core->SetPipeline(FRenderCoreProcess::ProcessType::AutoExposureAverage, context);
		core->BindComputeBuffer(FRenderCoreProcess::ProcessType::AutoExposureAverage, context, desc);
		context->GetCommandList()->Dispatch(1, 1, 1);

		averageLuminance_->Barrier(context->GetDxCommand());
	}

	{
		core->SetPipeline(FRenderCoreProcess::ProcessType::AutoExposureApply, context);
		core->BindComputeBuffer(FRenderCoreProcess::ProcessType::AutoExposureApply, context, desc);
		core->Dispatch(context, info.buffer->GetSize());
	}

	process->GetCurrentTexture()->TransitionEndUnordered(context);
}

void PostProcessAutoExposure::ShowInspectorImGui() {
	parameter_->At().SetImGuiCommand();

	ReadbackDimensionBuffer<uint32_t>::Readback(
		System::GetDxDevice(),
		System::GetDirectQueueContext()->GetDxCommand(),
		histgram_.get(),
		debugHistgram_.get()
	);

	ReadbackDimensionBuffer<float>::Readback(
		System::GetDxDevice(),
		System::GetDirectQueueContext()->GetDxCommand(),
		averageLuminance_.get(),
		debugAverageLuminance_.get()
	);

	uint32_t sum = std::accumulate(debugHistgram_->GetSpan().begin(), debugHistgram_->GetSpan().end(), 0);

	ImVec2 cursor = ImGui::GetCursorPos();
	ImVec2 size   = { ImGui::GetContentRegionAvail().x, 80.0f };

	SxImGui::PlotHistogramFunc(
		"## histogram",
		[&](uint32_t index) { return sum != 0 ? static_cast<float>(debugHistgram_->At(index)) / static_cast<float>(sum) : 0; },
		debugHistgram_->GetSize(),
		0,
		NULL,
		std::nullopt,
		std::nullopt,
		size
	);

	float avarage = debugAverageLuminance_->At(0);
	float t = (avarage - parameter_->At().minLogLuminance) / (parameter_->At().maxLogLuminance - parameter_->At().minLogLuminance);

	// FIXME
	ImGui::ProgressBar(t, { size.x, 0.0f }, std::format("avarage luminance: {}", avarage).c_str());

	
}

const PostProcessAutoExposure::Parameter& PostProcessAutoExposure::GetParameter() const {
	StreamLogger::AssertA(parameter_ != nullptr, "auto exposure parameter buffer is not create.");
	return parameter_->At();
}

PostProcessAutoExposure::Parameter& PostProcessAutoExposure::GetParameter() {
	StreamLogger::AssertA(parameter_ != nullptr, "auto exposure parameter buffer is not create.");
	return parameter_->At();
}
