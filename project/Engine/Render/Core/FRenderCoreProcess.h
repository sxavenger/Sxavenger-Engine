#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "FBaseRenderCore.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/Configuration/Configuration.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>
#include <Engine/Module/Pipeline/CustomComputePipeline.h>

//* lib
#include <Lib/Math/Vector2.h>

//* external
#include <magic_enum.hpp>

//* c++
#include <array>
#include <filesystem>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCoreProcess class
////////////////////////////////////////////////////////////////////////////////////////////
class FRenderCoreProcess final
	: public FBaseRenderCore {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// CompositeProcess enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class CompositeProcess : uint8_t {
		Tonemap,
		FXAA,
		SMAA_EdgeDetection,
		SMAA_BlendWeight,
		SMAA_NeighborhoodBlending,
		XeGTAO_PrefilterDepth,
		XeGTAO_Main,
		XeGTAO_DenoiseFirst,
		XeGTAO_DenoiseLast,
		XeGTAO_Resolve,
	};
	static const size_t	kCompositeProcessCount = magic_enum::enum_count<CompositeProcess>();

	////////////////////////////////////////////////////////////////////////////////////////////
	// PostProcess enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class PostProcess : uint32_t {
		GrayScale,
		Bloom_Luminance,
		Bloom_Apply,
		AutoExposure_Luminance,
		AutoExposure_Average,
		AutoExposure_Apply,
		LocalExposure,
		RadialBlur,
		MotionBlur,
		ChromaticAberration,
		Vignette,
		LUT_Convert,
		LUT,
		PostFxBrightness,
		PostFxSaturation,
		PostFxColorfulness,
	};
	static const size_t kPostProcessCount = magic_enum::enum_count<PostProcess>();

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init() override;

	//* process option *//

	void Dispatch(const DirectXQueueContext* context, const Vector2ui& resolution) const;

	//* post process option *//

	void SetPipeline(PostProcess process, const DirectXQueueContext* context) const;

	void BindComputeBuffer(PostProcess process, const DirectXQueueContext* context, const DxObject::BindBufferDesc& desc) const;

	//* composite process option *//

	void SetPipeline(CompositeProcess process, const DirectXQueueContext* context) const;

	void BindComputeBuffer(CompositeProcess process, const DirectXQueueContext* context, const DxObject::BindBufferDesc& desc) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* process pipelines *//

	std::array<CustomReflectionComputePipeline, kPostProcessCount> post_;
	std::array<CustomReflectionComputePipeline, kCompositeProcessCount> composite_;

	// TODO: Shader Script への移行

	//* process constants *//

	static inline const std::filesystem::path kDirectory = kPackagesDirectory / "shaders" / "render" / "Process";
	static inline const Vector2ui kNumthreads = { 16, 16 };

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreatePipeline();

	//* helper methods *//

	void CreatePipeline(PostProcess process, const std::filesystem::path& filepath);
	void CreatePipeline(PostProcess process, const std::filesystem::path& filepath, const DxObject::SamplerBindDesc& desc);

	void CreatePipeline(CompositeProcess process, const std::filesystem::path& filepath);
	void CreatePipeline(CompositeProcess process, const std::filesystem::path& filepath, const DxObject::SamplerBindDesc& desc);

};

SXAVENGER_ENGINE_NAMESPACE_END
