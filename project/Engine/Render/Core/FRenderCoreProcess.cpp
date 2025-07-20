#include "FRenderCoreProcess.h"

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCoreProcess class
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderCoreProcess::Init() {

	//* process *//

	//!< environment
	CreatePipeline(ProcessType::Environment, "PostProcess/Environment.cs.hlsl");

	//!< volumetric fog
	CreatePipeline(ProcessType::VolumetricFog, "PostProcess/volumetricFog.cs.hlsl");

	//!< bloom
	CreatePipeline(ProcessType::Bloom, "PostProcess/Bloom.cs.hlsl");

	//!< auto exposure
	CreatePipeline(ProcessType::AutoExposureLuminance, "PostProcess/Exposure/AutoExposureLuminance.cs.hlsl");
	CreatePipeline(ProcessType::AutoExposureAverage,   "PostProcess/Exposure/AutoExposureAverage.cs.hlsl");
	CreatePipeline(ProcessType::AutoExposureApply,     "PostProcess/Exposure/AutoExposureApply.cs.hlsl");
	//!< local exposure
	CreatePipeline(ProcessType::LocalExposure, "PostProcess/Exposure/LocalExposure.cs.hlsl");

	//!< dof
	CreatePipeline(ProcessType::DoF, "PostProcess/DoF.cs.hlsl");

	//!< vignette
	CreatePipeline(ProcessType::Vignette, "PostProcess/Vignette.cs.hlsl");

	//* composite *//

	//!< convert lut texture
	CreatePipeline(CompositeType::ConvertLUTTexture, "CompositeProcess/ConvertLUTTexture.cs.hlsl");

	//!< lut
	{
		DxObject::SamplerBindDesc desc = {};
		desc.SetSamplerLinear("gLUTSampler", DxObject::SamplerMode::MODE_CLAMP);

		CreatePipeline(CompositeType::LUT, "CompositeProcess/LUT.cs.hlsl", desc);
	}

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
	process->CreateAsset(kDirectory_ / filepath);
	process->RegisterBlob();
	process->ReflectionPipeline(SxavengerSystem::GetDxDevice());

	processes_[static_cast<uint32_t>(type)] = std::move(process);
}

void FRenderCoreProcess::CreatePipeline(ProcessType type, const std::filesystem::path& filepath, const DxObject::SamplerBindDesc& desc) {
	auto process = std::make_unique<CustomReflectionComputePipeline>();
	process->CreateAsset(kDirectory_ / filepath);
	process->RegisterBlob();
	process->ReflectionPipeline(SxavengerSystem::GetDxDevice(), desc);

	processes_[static_cast<uint32_t>(type)] = std::move(process);
}

void FRenderCoreProcess::CreatePipeline(CompositeType type, const std::filesystem::path& filepath) {
	auto process = std::make_unique<CustomReflectionComputePipeline>();
	process->CreateAsset(kDirectory_ / filepath);
	process->RegisterBlob();
	process->ReflectionPipeline(SxavengerSystem::GetDxDevice());

	composites_[static_cast<uint32_t>(type)] = std::move(process);
}

void FRenderCoreProcess::CreatePipeline(CompositeType type, const std::filesystem::path& filepath, const DxObject::SamplerBindDesc& desc) {
	auto process = std::make_unique<CustomReflectionComputePipeline>();
	process->CreateAsset(kDirectory_ / filepath);
	process->RegisterBlob();
	process->ReflectionPipeline(SxavengerSystem::GetDxDevice(), desc);

	composites_[static_cast<uint32_t>(type)] = std::move(process);
}
