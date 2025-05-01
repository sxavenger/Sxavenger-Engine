#include "FRenderCoreProcess.h"

//=========================================================================================
// static variables
//=========================================================================================

const Vector2ui FRenderCoreProcess::kNumThreadSize_ = { 16, 16 };

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCoreProcess class
////////////////////////////////////////////////////////////////////////////////////////////

void FRenderCoreProcess::Init() {

	{ //!< nlao
		auto process = std::make_unique<CustomReflectionComputePipeline>();
		process->CreateAsset(L"packages/shaders/render/AmbientProcess/NLAO.cs.hlsl");
		process->RegisterBlob();
		process->ReflectionPipeline(SxavengerSystem::GetDxDevice());

		processes_[static_cast<uint32_t>(ProcessType::NLAO)] = std::move(process);
	}

	{ //!< nlao blur
		auto process = std::make_unique<CustomReflectionComputePipeline>();
		process->CreateAsset(L"packages/shaders/render/AmbientProcess/NLAOBlur.cs.hlsl");
		process->RegisterBlob();
		process->ReflectionPipeline(SxavengerSystem::GetDxDevice());

		processes_[static_cast<uint32_t>(ProcessType::NLAO_Blur)] = std::move(process);
	}

	{ //!< overlay
		auto process = std::make_unique<CustomReflectionComputePipeline>();
		process->CreateAsset(L"packages/shaders/render/PostProcess/Overlay.cs.hlsl");
		process->RegisterBlob();
		process->ReflectionPipeline(SxavengerSystem::GetDxDevice());

		processes_[static_cast<uint32_t>(ProcessType::Overlay)] = std::move(process);
	}

	{ //!< bloom
		auto process = std::make_unique<CustomReflectionComputePipeline>();
		process->CreateAsset(L"packages/shaders/render/PostProcess/Bloom.cs.hlsl");
		process->RegisterBlob();
		process->ReflectionPipeline(SxavengerSystem::GetDxDevice());

		processes_[static_cast<uint32_t>(ProcessType::Bloom)] = std::move(process);
	}

	{ //!< exposure
		auto process = std::make_unique<CustomReflectionComputePipeline>();
		process->CreateAsset(L"packages/shaders/render/PostProcess/Exposure.cs.hlsl");
		process->RegisterBlob();
		process->ReflectionPipeline(SxavengerSystem::GetDxDevice());

		processes_[static_cast<uint32_t>(ProcessType::Exposure)] = std::move(process);
	}

	{ //!< lut
		auto process = std::make_unique<CustomReflectionComputePipeline>();
		process->CreateAsset(L"packages/shaders/render/PostProcess/LUT.cs.hlsl");
		process->RegisterBlob();
		process->ReflectionPipeline(SxavengerSystem::GetDxDevice());

		processes_[static_cast<uint32_t>(ProcessType::LUT)] = std::move(process);
	}

	{ //!< dof
		auto process = std::make_unique<CustomReflectionComputePipeline>();
		process->CreateAsset(L"packages/shaders/render/PostProcess/DoF.cs.hlsl");
		process->RegisterBlob();
		process->ReflectionPipeline(SxavengerSystem::GetDxDevice());

		processes_[static_cast<uint32_t>(ProcessType::DoF)] = std::move(process);
	}

	{ //!< vignette
		auto process = std::make_unique<CustomReflectionComputePipeline>();
		process->CreateAsset(L"packages/shaders/render/PostProcess/Vignette.cs.hlsl");
		process->RegisterBlob();
		process->ReflectionPipeline(SxavengerSystem::GetDxDevice());

		processes_[static_cast<uint32_t>(ProcessType::Vignette)] = std::move(process);
	}
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
