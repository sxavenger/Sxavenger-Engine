#include "FRenderCoreProcess.h"

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCoreProcess class
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderCoreProcess::Init() {

	//* process *//

	//!< volumetric fog
	//CreatePipeline(ProcessType::VolumetricFog, "PostProcess/volumetricFog.cs.hlsl");

	//!< grayscale
	CreatePipeline(ProcessType::GrayScale, "PostProcess/GrayScale.cs.hlsl");

	//!< auto exposure
	CreatePipeline(ProcessType::AutoExposureLuminance, "PostProcess/Exposure/AutoExposureLuminance.cs.hlsl");
	CreatePipeline(ProcessType::AutoExposureAverage,   "PostProcess/Exposure/AutoExposureAverage.cs.hlsl");
	CreatePipeline(ProcessType::AutoExposureApply,     "PostProcess/Exposure/AutoExposureApply.cs.hlsl");
	//!< local exposure
	CreatePipeline(ProcessType::LocalExposure, "PostProcess/Exposure/LocalExposure.cs.hlsl");

	//!< dof
	CreatePipeline(ProcessType::DoF, "PostProcess/DoF.cs.hlsl");

	
	{
		DxObject::SamplerBindDesc desc = {};
		desc.SetSamplerLinear("gSampler", DxObject::SamplerMode::MODE_CLAMP);

		//!< bloom
		CreatePipeline(ProcessType::BloomLuminance, "PostProcess/Bloom/BloomLuminance.cs.hlsl", desc);
		CreatePipeline(ProcessType::BloomApply,     "PostProcess/Bloom/BloomApply.cs.hlsl",     desc);

		//!< radial blur
		CreatePipeline(ProcessType::RadialBlur, "PostProcess/RadialBlur.cs.hlsl", desc);

		//!< chromatic aberration
		CreatePipeline(ProcessType::ChromaticAberration, "PostProcess/ChromaticAberration.cs.hlsl", desc);
	}
	

	//!< vignette
	CreatePipeline(ProcessType::Vignette, "PostProcess/Vignette.cs.hlsl");

	//!< convert lut texture
	CreatePipeline(ProcessType::ConvertLUTTexture, "PostProcess/ConvertLUTTexture.cs.hlsl");

	//!< lut
	{
		DxObject::SamplerBindDesc desc = {};
		desc.SetSamplerLinear("gLUTSampler", DxObject::SamplerMode::MODE_CLAMP);

		CreatePipeline(ProcessType::LUT, "PostProcess/LUT.cs.hlsl", desc);
	}

	//* composite *//

	//!< environment
	CreatePipeline(CompositeType::Environment, "CompositeProcess/Environment.cs.hlsl");

	//!< tonemap
	CreatePipeline(CompositeType::Tonemap, "CompositeProcess/Tonemap.cs.hlsl");

}

void FRenderCoreProcess::SetPipeline(ProcessType type, const DirectXQueueContext* context) {
	processes_[static_cast<uint32_t>(type)]->SetPipeline(context->GetDxCommand());
}

void FRenderCoreProcess::BindComputeBuffer(ProcessType type, const DirectXQueueContext* context, const DxObject::BindBufferDesc& desc) {
	processes_[static_cast<uint32_t>(type)]->BindComputeBuffer(context->GetDxCommand(), desc);
}

void FRenderCoreProcess::SetPipeline(CompositeType type, const DirectXQueueContext* context) {
	composites_[static_cast<uint32_t>(type)]->SetPipeline(context->GetDxCommand());
}

void FRenderCoreProcess::BindComputeBuffer(CompositeType type, const DirectXQueueContext* context, const DxObject::BindBufferDesc& desc) {
	composites_[static_cast<uint32_t>(type)]->BindComputeBuffer(context->GetDxCommand(), desc);
}

void FRenderCoreProcess::Dispatch(const DirectXQueueContext* context, const Vector2ui& size) const {
	context->GetCommandList()->Dispatch(DxObject::RoundUp(size.x, kNumThreadSize_.x), DxObject::RoundUp(size.y, kNumThreadSize_.y), 1);
}

void FRenderCoreProcess::CreatePipeline(ProcessType type, const std::filesystem::path& filepath) {
	auto process = std::make_unique<CustomReflectionComputePipeline>();
	process->CreateContent(kDirectory_ / filepath);
	process->RegisterBlob();
	process->ReflectionPipeline(SxavengerSystem::GetDxDevice());

	processes_[static_cast<uint32_t>(type)] = std::move(process);
}

void FRenderCoreProcess::CreatePipeline(ProcessType type, const std::filesystem::path& filepath, const DxObject::SamplerBindDesc& desc) {
	auto process = std::make_unique<CustomReflectionComputePipeline>();
	process->CreateContent(kDirectory_ / filepath);
	process->RegisterBlob();
	process->ReflectionPipeline(SxavengerSystem::GetDxDevice(), desc);

	processes_[static_cast<uint32_t>(type)] = std::move(process);
}

void FRenderCoreProcess::CreatePipeline(CompositeType type, const std::filesystem::path& filepath) {
	auto process = std::make_unique<CustomReflectionComputePipeline>();
	process->CreateContent(kDirectory_ / filepath);
	process->RegisterBlob();
	process->ReflectionPipeline(SxavengerSystem::GetDxDevice());

	composites_[static_cast<uint32_t>(type)] = std::move(process);
}

void FRenderCoreProcess::CreatePipeline(CompositeType type, const std::filesystem::path& filepath, const DxObject::SamplerBindDesc& desc) {
	auto process = std::make_unique<CustomReflectionComputePipeline>();
	process->CreateContent(kDirectory_ / filepath);
	process->RegisterBlob();
	process->ReflectionPipeline(SxavengerSystem::GetDxDevice(), desc);

	composites_[static_cast<uint32_t>(type)] = std::move(process);
}
