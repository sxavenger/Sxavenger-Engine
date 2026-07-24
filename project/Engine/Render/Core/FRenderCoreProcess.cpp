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
	desc.SetSamplerLinear("gWarpSampler", DxObject::SamplerMode::Wrap);


	{ //!< Post Process

		//!< GrayScale
		CreatePipeline(PostProcess::GrayScale, "GrayScale.cs.hlsl");

		//!< Auto Exposure
		CreatePipeline(PostProcess::AutoExposure_Luminance, "Exposure/AutoExposureLuminance.cs.hlsl");
		CreatePipeline(PostProcess::AutoExposure_Average,   "Exposure/AutoExposureAverage.cs.hlsl");
		CreatePipeline(PostProcess::AutoExposure_Apply,     "Exposure/AutoExposureApply.cs.hlsl");

		//!< Local Exposure
		CreatePipeline(PostProcess::LocalExposure, "Exposure/LocalExposure.cs.hlsl");

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

		//!< PostFx
		CreatePipeline(PostProcess::PostFxBrightness,   "PostFx/PostFxBrightness.cs.hlsl", desc);
		CreatePipeline(PostProcess::PostFxSaturation,   "PostFx/PostFxSaturation.cs.hlsl", desc);
		CreatePipeline(PostProcess::PostFxColorfulness, "PostFx/PostFxColorfulness.cs.hlsl", desc);

		//!< Posterize
		CreatePipeline(PostProcess::Posterize, "Posterize/Posterize.cs.hlsl", desc);

		//!< Sketch
		CreatePipeline(PostProcess::Sketch, "Sketch/Sketch.cs.hlsl", desc);
	}

	{ //!< Composite Process

		//!< Tonemap
		CreatePipeline(CompositeProcess::Tonemap, "Tonemap.cs.hlsl");

		//!< FXAA
		CreatePipeline(CompositeProcess::FXAA, "FXAA/Main.cs.hlsl", desc);

		//!< SMAA
		CreatePipeline(CompositeProcess::SMAA_EdgeDetection,        "SMAA/EdgeDetection.cs.hlsl",        desc);
		CreatePipeline(CompositeProcess::SMAA_BlendWeight,          "SMAA/BlendWeight.cs.hlsl",          desc);
		CreatePipeline(CompositeProcess::SMAA_NeighborhoodBlending, "SMAA/NeighborhoodBlending.cs.hlsl", desc);

		//!< XeGTAO
		CreatePipeline(CompositeProcess::XeGTAO_PrefilterDepth, "XeGTAO/PrefilterDepth.cs.hlsl", desc);
		CreatePipeline(CompositeProcess::XeGTAO_Main,           "XeGTAO/Main.cs.hlsl",           desc);
		CreatePipeline(CompositeProcess::XeGTAO_DenoiseFirst,   "XeGTAO/DenoiseFirst.cs.hlsl",   desc);
		CreatePipeline(CompositeProcess::XeGTAO_DenoiseLast,    "XeGTAO/DenoiseLast.cs.hlsl",    desc);
		CreatePipeline(CompositeProcess::XeGTAO_Resolve,        "XeGTAO/Resolve.cs.hlsl",        desc);
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
