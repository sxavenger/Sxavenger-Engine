#pragma once
//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Config/SxavengerConfig.h>
#include <Engine/Module/Pipeline/CustomComputePipeline.h>

//* external
#include <magic_enum.hpp>

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
		GrayScale,
		BloomLuminance,
		BloomApply,
		AutoExposureLuminance,
		AutoExposureAverage,
		AutoExposureApply,
		LocalExposure,
		DoF,
		RadialBlur,
		MotionBlur,
		ChromaticAberration,
		Vignette,
		ConvertLUTTexture,
		LUT
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// CompositeType enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class CompositeType : uint32_t {
		Tonemap,
		FXAA,
	};

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

	std::array<std::unique_ptr<CustomReflectionComputePipeline>, magic_enum::enum_count<ProcessType>()> processes_;
	std::array<std::unique_ptr<CustomReflectionComputePipeline>, magic_enum::enum_count<CompositeType>()> composites_;

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
