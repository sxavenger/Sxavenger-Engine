#pragma once
//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Config/SxavengerConfig.h>
#include <Engine/Module/Pipeline/CustomComputePipeline.h>

//* c++
#include <array>

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCoreProcess class
////////////////////////////////////////////////////////////////////////////////////////////
class FRenderCoreProcess {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// ProcessType enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class ProcessType : uint32_t {
		Environment,
		VolumetricFog,
		Bloom,
		AutoExposureLuminance,
		AutoExposureAverage,
		AutoExposureApply,
		LocalExposure,
		DoF,
		Vignette,
	};
	static inline const uint32_t kProcessTypeCount = static_cast<uint32_t>(ProcessType::Vignette) + 1;

	////////////////////////////////////////////////////////////////////////////////////////////
	// CompositeType enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class CompositeType : uint32_t {
		ColorCurve,
		ConvertLUTTexture,
		LUT,
		Tonemap
	};
	static inline const uint32_t kCompositeTypeCount = static_cast<uint32_t>(CompositeType::Tonemap) + 1;


public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FRenderCoreProcess()  = default;
	~FRenderCoreProcess() = default;

	void Init();

	void Dispatch(const DirectXQueueContext* context, const Vector2ui& size) const;

	//* process option *//

	void SetPipeline(ProcessType type, const DirectXQueueContext* context);

	void BindComputeBuffer(ProcessType type, const DirectXQueueContext* context, const DxObject::BindBufferDesc& desc);

	//* composite option *//

	void SetPipeline(CompositeType type, const DirectXQueueContext* context);

	void BindComputeBuffer(CompositeType type, const DirectXQueueContext* context, const DxObject::BindBufferDesc& desc);


private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::array<std::unique_ptr<CustomReflectionComputePipeline>, kProcessTypeCount> processes_;
	std::array<std::unique_ptr<CustomReflectionComputePipeline>, kCompositeTypeCount> composites_;

	static inline const Vector2ui kNumThreadSize_ = { 16, 16 };

	static inline const std::filesystem::path kDirectory_ = kPackagesShaderDirectory / "Render" / "Process";

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreatePipeline(ProcessType type, const std::filesystem::path& filepath);
	void CreatePipeline(ProcessType type, const std::filesystem::path& filepath, const DxObject::SamplerBindDesc& desc);

	void CreatePipeline(CompositeType type, const std::filesystem::path& filepath);
	void CreatePipeline(CompositeType type, const std::filesystem::path& filepath, const DxObject::SamplerBindDesc& desc);

};
