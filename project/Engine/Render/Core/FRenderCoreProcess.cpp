#include "FRenderCoreProcess.h"

//=========================================================================================
// static variables
//=========================================================================================

const Vector2ui FRenderCoreProcess::kNumThreadSize_ = { 16, 16 };

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCoreProcess class
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderCoreProcess::Init() {

	//{ //!< nlao
	//	auto process = std::make_unique<CustomReflectionComputePipeline>();
	//	process->CreateAsset(L"packages/shaders/render/AmbientProcess/NLAO.cs.hlsl");
	//	process->RegisterBlob();
	//	process->ReflectionPipeline(SxavengerSystem::GetDxDevice());

	//	processes_[static_cast<uint32_t>(ProcessType::NLAO)] = std::move(process);
	//}

	//{ //!< nlao blur
	//	auto process = std::make_unique<CustomReflectionComputePipeline>();
	//	process->CreateAsset(L"packages/shaders/render/AmbientProcess/NLAOBlur.cs.hlsl");
	//	process->RegisterBlob();
	//	process->ReflectionPipeline(SxavengerSystem::GetDxDevice());

	//	processes_[static_cast<uint32_t>(ProcessType::NLAO_Blur)] = std::move(process);
	//}

	//{ //!< overlay
	//	auto process = std::make_unique<CustomReflectionComputePipeline>();
	//	process->CreateAsset(L"packages/shaders/render/PostProcess/Overlay.cs.hlsl");
	//	process->RegisterBlob();
	//	process->ReflectionPipeline(SxavengerSystem::GetDxDevice());

	//	processes_[static_cast<uint32_t>(ProcessType::Overlay)] = std::move(process);
	//}

	//!< environment
	CreatePipeline(ProcessType::Environment, L"packages/shaders/render/PostProcess/Environment.cs.hlsl");

	//!< bloom
	CreatePipeline(ProcessType::Bloom, L"packages/shaders/render/PostProcess/Bloom.cs.hlsl");

	//!< exposure
	CreatePipeline(ProcessType::Exposure, L"packages/shaders/render/PostProcess/Exposure.cs.hlsl");

	//!< lut
	CreatePipeline(ProcessType::LUT, L"packages/shaders/render/PostProcess/LUT.cs.hlsl");

	//!< dof
	CreatePipeline(ProcessType::DoF, L"packages/shaders/render/PostProcess/DoF.cs.hlsl");

	//!< vignette
	CreatePipeline(ProcessType::Vignette, L"packages/shaders/render/PostProcess/Vignette.cs.hlsl");
}

void FRenderCoreProcess::SetPipeline(ProcessType type, const DirectXThreadContext* context) {
	processes_[static_cast<uint32_t>(type)]->SetPipeline(context->GetDxCommand());
}

void FRenderCoreProcess::BindComputeBuffer(ProcessType type, const DirectXThreadContext* context, const DxObject::BindBufferDesc& desc) {
	processes_[static_cast<uint32_t>(type)]->BindComputeBuffer(context->GetDxCommand(), desc);
}

void FRenderCoreProcess::Dispatch(const DirectXThreadContext* context, const Vector2ui& size) const {
	context->GetCommandList()->Dispatch(DxObject::RoundUp(size.x, kNumThreadSize_.x), DxObject::RoundUp(size.y, kNumThreadSize_.y), 1);
}

void FRenderCoreProcess::CreatePipeline(ProcessType type, const std::filesystem::path& filepath) {
	auto process = std::make_unique<CustomReflectionComputePipeline>();
	process->CreateAsset(filepath);
	process->RegisterBlob();
	process->ReflectionPipeline(SxavengerSystem::GetDxDevice());

	processes_[static_cast<uint32_t>(type)] = std::move(process);
}
