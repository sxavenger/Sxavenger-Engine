#include "FRenderCoreProcess.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/System.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCoreProcess class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderCoreProcess::Init() {
	CreatePipeline();
}

void FRenderCoreProcess::Dispatch(const DirectXQueueContext* context, const Vector2ui& resolution) const {
	context->GetCommandList()->Dispatch(DxObject::RoundUp(resolution.x, kNumthreads.x), DxObject::RoundUp(resolution.y, kNumthreads.y), 1);
}

void FRenderCoreProcess::SetPipeline(PostProcess process, const DirectXQueueContext* context) const {
	post_[static_cast<uint32_t>(process)].SetPipeline(context->GetDxCommand());
}

void FRenderCoreProcess::BindComputeBuffer(PostProcess process, const DirectXQueueContext* context, const DxObject::BindBufferDesc& desc) const {
	post_[static_cast<uint32_t>(process)].BindComputeBuffer(context->GetDxCommand(), desc);
}

void FRenderCoreProcess::SetPipeline(CompositeProcess process, const DirectXQueueContext* context) const {
	composite_[static_cast<uint32_t>(process)].SetPipeline(context->GetDxCommand());
}

void FRenderCoreProcess::BindComputeBuffer(CompositeProcess process, const DirectXQueueContext* context, const DxObject::BindBufferDesc& desc) const {
	composite_[static_cast<uint32_t>(process)].BindComputeBuffer(context->GetDxCommand(), desc);
}

void FRenderCoreProcess::CreatePipeline() {

	DxObject::SamplerBindDesc desc = {};
	desc.SetSamplerLinear("gSampler", DxObject::SamplerMode::Clamp);
	desc.SetSamplerLinear("gLUTSampler", DxObject::SamplerMode::Clamp);
	desc.SetSamplerLinear("gLinearSampler", DxObject::SamplerMode::Clamp);
	desc.SetSamplerPoint("gPointSampler", DxObject::SamplerMode::Clamp);


	{ //!< Post Process

		//!< GrayScale
		CreatePipeline(PostProcess::GrayScale, "GrayScale.cs.hlsl");

		//!< Auto Exposure
		CreatePipeline(PostProcess::AutoExposure_Luminance, "Exposure/AutoExposureLuminance.cs.hlsl");
		CreatePipeline(PostProcess::AutoExposure_Average,   "Exposure/AutoExposureAverage.cs.hlsl");
		CreatePipeline(PostProcess::AutoExposure_Apply,     "Exposure/AutoExposureApply.cs.hlsl");

		//!< Local Exposure
		CreatePipeline(PostProcess::LocalExposure, "Exposure/LocalExposure.cs.hlsl");

		//!< Depth of Field
		CreatePipeline(PostProcess::DepthOfField, "DoF.cs.hlsl");
		// FIXME: Depth of Fieldの処理の修正.

		//!< Bloom
		CreatePipeline(PostProcess::Bloom_Luminance, "Bloom/BloomLuminance.cs.hlsl", desc);
		CreatePipeline(PostProcess::Bloom_Apply,     "Bloom/BloomApply.cs.hlsl",     desc);

		//!< Radial Blur
		CreatePipeline(PostProcess::RadialBlur, "RadialBlur.cs.hlsl", desc);

		//!< Motion Blur
		CreatePipeline(PostProcess::MotionBlur, "MotionBlur.cs.hlsl", desc);

		//!< Chromatic Aberration
		CreatePipeline(PostProcess::ChromaticAberration, "ChromaticAberration.cs.hlsl", desc);

		//!< Vignette
		CreatePipeline(PostProcess::Vignette, "Vignette.cs.hlsl", desc);

		//!< LUT
		CreatePipeline(PostProcess::LUT_Convert, "LUT/ConvertLUTTexture.cs.hlsl", desc);
		CreatePipeline(PostProcess::LUT,         "LUT/LUT.cs.hlsl", desc);
	}

	{ //!< Composite Process

		//!< Tonemap
		CreatePipeline(CompositeProcess::Tonemap, "Tonemap.cs.hlsl");

		//!< FXAA
		CreatePipeline(CompositeProcess::FXAA, "FXAA/Fxaa.cs.hlsl", desc);

		//!< SMAA
		CreatePipeline(CompositeProcess::SMAA_EdgeDetection,        "SMAA/SmaaEdgeDetection.cs.hlsl",        desc);
		CreatePipeline(CompositeProcess::SMAA_BlendWeight,          "SMAA/SmaaBlendWeight.cs.hlsl",          desc);
		CreatePipeline(CompositeProcess::SMAA_NeighborhoodBlending, "SMAA/SmaaNeighborhoodBlending.cs.hlsl", desc);

	}

}

void FRenderCoreProcess::CreatePipeline(PostProcess process, const std::filesystem::path& filepath) {
	auto& pipeline = post_[static_cast<uint32_t>(process)];
	pipeline.CreateContent(kDirectory / "PostProcess" / filepath);
	pipeline.RegisterBlob();
	pipeline.ReflectionPipeline(System::GetDxDevice());
}

void FRenderCoreProcess::CreatePipeline(PostProcess process, const std::filesystem::path& filepath, const DxObject::SamplerBindDesc& desc) {
	auto& pipeline = post_[static_cast<uint32_t>(process)];
	pipeline.CreateContent(kDirectory / "PostProcess" / filepath);
	pipeline.RegisterBlob();
	pipeline.ReflectionPipeline(System::GetDxDevice(), desc);
}

void FRenderCoreProcess::CreatePipeline(CompositeProcess process, const std::filesystem::path& filepath) {
	auto& pipeline = composite_[static_cast<uint32_t>(process)];
	pipeline.CreateContent(kDirectory / "CompositeProcess" / filepath);
	pipeline.RegisterBlob();
	pipeline.ReflectionPipeline(System::GetDxDevice());
}

void FRenderCoreProcess::CreatePipeline(CompositeProcess process, const std::filesystem::path& filepath, const DxObject::SamplerBindDesc& desc) {
	auto& pipeline = composite_[static_cast<uint32_t>(process)];
	pipeline.CreateContent(kDirectory / "CompositeProcess" / filepath);
	pipeline.RegisterBlob();
	pipeline.ReflectionPipeline(System::GetDxDevice(), desc);
}
