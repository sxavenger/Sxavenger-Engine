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
#include <Engine/Module/Pipeline/CustomGraphicsPipeline.h>
#include <Engine/Module/Pipeline/CustomComputePipeline.h>

//* lib
#include <Lib/Math/Vector2.h>
#include <Lib/Adapter/Random/Random.h>

//* external
#include <magic_enum.hpp>

//* c++
#include <array>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCoreSkyLight class
////////////////////////////////////////////////////////////////////////////////////////////
class FRenderCoreSkyLight final
	: public FBaseRenderCore {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Pipeline enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Pipeline : uint8_t {
		InitialReservoir,
		SpatialReservoir,
		History,
		Solve,
	};
	static inline const size_t kPipelineCount = magic_enum::enum_count<Pipeline>();

	////////////////////////////////////////////////////////////////////////////////////////////
	// Seed structure
	////////////////////////////////////////////////////////////////////////////////////////////
	template <size_t N>
	struct Seed {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		Seed() { Set(); }

		void Set() { std::generate(seed.begin(), seed.end(), []() { return Random::UniformDistribution<uint32_t>(std::numeric_limits<uint32_t>::lowest(), std::numeric_limits<uint32_t>::max()); }); }

		uint32_t* Data() { return seed.data(); }

		//=========================================================================================
		// public variables
		//=========================================================================================

		std::array<uint32_t, N> seed;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init() override;

	//* pipeline option *//

	void SetPipeline(Pipeline pipeline, const DirectXQueueContext* context) const;

	void BindComputeBuffer(Pipeline pipeline, const DirectXQueueContext* context, const DxObject::BindBufferDesc& desc) const;

	void Dispatch(const DirectXQueueContext* context, const Vector2ui& resolution) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* compute pipeline *//

	std::array<CustomReflectionComputePipeline, kPipelineCount> pipelines_ = {};

	static inline const Vector3ui kThreadGroupSize = { 16, 16, 1 };

	//* directory *//

	static inline const std::filesystem::path kDirectory = kPackagesDirectory / "shaders" / "render" / "Sky";

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreatePipeline();

	//* helper methods *//

	void CreateComputePipeline(Pipeline pipeline, std::filesystem::path compute);

};

SXAVENGER_ENGINE_NAMESPACE_END
