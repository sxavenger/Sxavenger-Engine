#pragma once
//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Module/Pipeline/CustomComputePipeline.h>

//* c++
#include <array>

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCoreProcess class
////////////////////////////////////////////////////////////////////////////////////////////
class FRenderCoreProcess {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// FRenderCoreProcess class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class ProcessType : uint32_t {
		Environment,
		Bloom,
		LUT,
		TextureLUT,
		Exposure,
		DoF,
		Vignette,
		Tonemap,
	};
	static const uint32_t kProcessTypeCount = static_cast<uint32_t>(ProcessType::Tonemap) + 1;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FRenderCoreProcess()  = default;
	~FRenderCoreProcess() = default;

	void Init();

	//* option *//

	void SetPipeline(ProcessType type, const DirectXThreadContext* context);

	void BindComputeBuffer(ProcessType type, const DirectXThreadContext* context, const DxObject::BindBufferDesc& desc);

	void Dispatch(const DirectXThreadContext* context, const Vector2ui& size) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::array<std::unique_ptr<CustomReflectionComputePipeline>, kProcessTypeCount> processes_;

	static const Vector2ui kNumThreadSize_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreatePipeline(ProcessType type, const std::filesystem::path& filepath);
	void CreatePipeline(ProcessType type, const std::filesystem::path& filepath, const DxObject::SamplerBindDesc& desc);

};
