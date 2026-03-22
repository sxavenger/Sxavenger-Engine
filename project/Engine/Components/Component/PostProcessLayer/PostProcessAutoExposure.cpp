#include "PostProcessAutoExposure.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/StreamLogger.h>
#include <Engine/System/UI/SxImGui.h>
#include <Engine/System/System.h>
#include <Engine/Render/Buffer/FRenderTargetBuffer.h>
#include <Engine/Render/Core/FRenderCore.h>
#include <Engine/Render/Core/FRenderCoreProcess.h>

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

	parameter_ = std::make_unique<DxObject::ConstantBuffer<Parameter>>();
	parameter_->Create(System::GetDxDevice());
	parameter_->At().Init();

	histogram_ = std::make_unique<DxObject::UnorderedDimensionBuffer<uint32_t>>();
	histogram_->Create(System::GetDxDevice(), kGroupCount_);

	histogramShared_ = std::make_unique<DxObject::UnorderedDimensionBuffer<uint32_t>>();
	histogramShared_->Create(System::GetDxDevice(), kGroupCount_);

	averageLuminance_ = std::make_unique<DxObject::UnorderedDimensionBuffer<float>>();
	averageLuminance_->Create(System::GetDxDevice(), 1);

	debugHistogram_         = std::make_unique<DxObject::ReadbackDimensionBuffer<uint32_t>>();
	debugAverageLuminance_ = std::make_unique<DxObject::ReadbackDimensionBuffer<float>>();
}

void PostProcessAutoExposure::Process(const DirectXQueueContext* context, const ProcessInfo& info) {
	// TODO: 完全に黒の場合は, Histogramから除外させる.

	FProcessBuffer* process = info.buffer->GetProcess();

	auto core = FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreProcess>(); //!< RenderCoreの確保.

	process->Next();
	process->GetCurrentTexture().TransitionUnorderedAccess(context);

	//!< parameterの設定
	DxObject::BindBufferDesc desc = {};

	//!< 共通parameterの設定
	desc.Set32bitConstants("Dimension", 2,   &info.buffer->GetResolution());
	desc.Set32bitConstants("Information", 1, &info.weight);

	//!< Bufferの設定
	desc.SetHandle("gInput",   process->GetPreviousTexture().GetGPUHandleSRV());
	desc.SetHandle("gOutput",  process->GetCurrentTexture().GetGPUHandleUAV());

	//!< parameterの設定
	desc.SetAddress("gHistogram",        histogram_->GetGPUVirtualAddress());
	desc.SetAddress("gHistogramShared",  histogramShared_->GetGPUVirtualAddress());
	desc.SetAddress("gAverageLuminance", averageLuminance_->GetGPUVirtualAddress());
	desc.SetAddress("gParameter",        parameter_->GetGPUVirtualAddress());

	{ //!< Luminance
		core->SetPipeline(FRenderCoreProcess::PostProcess::AutoExposure_Luminance, context);
		core->BindComputeBuffer(FRenderCoreProcess::PostProcess::AutoExposure_Luminance, context, desc);
		core->Dispatch(context, info.buffer->GetResolution());

		histogramShared_->Barrier(context->GetDxCommand());
	}

	{ //!< Average 
		core->SetPipeline(FRenderCoreProcess::PostProcess::AutoExposure_Average, context);
		core->BindComputeBuffer(FRenderCoreProcess::PostProcess::AutoExposure_Average, context, desc);
		context->GetCommandList()->Dispatch(1, 1, 1);

		averageLuminance_->Barrier(context->GetDxCommand());
	}

	{ //!< Apply
		core->SetPipeline(FRenderCoreProcess::PostProcess::AutoExposure_Apply, context);
		core->BindComputeBuffer(FRenderCoreProcess::PostProcess::AutoExposure_Apply, context, desc);
		core->Dispatch(context, info.buffer->GetResolution());
	}

	process->GetCurrentTexture().TransitionDefaultState(context);
}

void PostProcessAutoExposure::ShowInspectorImGui() {
	parameter_->At().SetImGuiCommand();

	debugHistogram_->Readback(
		System::GetDxDevice(),
		System::GetDirectQueueContext()->GetDxCommand(),
		histogram_.get()
	);

	debugAverageLuminance_->Readback(
		System::GetDxDevice(),
		System::GetDirectQueueContext()->GetDxCommand(),
		averageLuminance_.get()
	);

	uint32_t sum = std::accumulate(debugHistogram_->GetSpan().begin(), debugHistogram_->GetSpan().end(), 0);

	ImVec2 cursor = ImGui::GetCursorPos();
	ImVec2 size   = { ImGui::GetContentRegionAvail().x, 80.0f };

	SxImGui::PlotHistogramFunc(
		"## histogram",
		[&](uint32_t index) { return sum != 0 ? static_cast<float>(debugHistogram_->At(index)) / static_cast<float>(sum) : 0; },
		debugHistogram_->GetSize(),
		0,
		NULL,
		std::nullopt,
		std::nullopt,
		size
	);

	float average = debugAverageLuminance_->At(0);
	float t = (average - parameter_->At().minLogLuminance) / (parameter_->At().maxLogLuminance - parameter_->At().minLogLuminance);

	// FIXME
	ImGui::ProgressBar(t, { size.x, 0.0f }, std::format("average luminance: {}", average).c_str());

	
}

const PostProcessAutoExposure::Parameter& PostProcessAutoExposure::GetParameter() const {
	StreamLogger::AssertA(parameter_ != nullptr, "auto exposure parameter buffer is not create.");
	return parameter_->At();
}

PostProcessAutoExposure::Parameter& PostProcessAutoExposure::GetParameter() {
	StreamLogger::AssertA(parameter_ != nullptr, "auto exposure parameter buffer is not create.");
	return parameter_->At();
}
