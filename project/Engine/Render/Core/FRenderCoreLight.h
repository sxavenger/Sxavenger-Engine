#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Foundation.h>
#include <Engine/System/Configuration/Configuration.h>
#include <Engine/Graphics/InputAssembler/TriangleInputAssembler.h>
#include <Engine/Module/Pipeline/CustomGraphicsPipeline.h>

//* external
#include <magic_enum.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCoreLight class
////////////////////////////////////////////////////////////////////////////////////////////
class FRenderCoreLight {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// LightType enum
	////////////////////////////////////////////////////////////////////////////////////////////
	enum LightType : uint8_t {
		Empty,

		//!< Punctual light
		Directional,
		Point,
		Spot,

		//!< Sky light
		SkyLight,
		SkyLightEnvironment,
		SkyAtmosphereEnvironment,
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init();

	//* input assembler option *//

	void DrawCall(const DirectXQueueContext* context, UINT instanceCount = 1) const;

	//* pipeline option *//

	void SetPipeline(LightType type, const DirectXQueueContext* context, const Vector2ui& size);

	void BindGraphicsBuffer(LightType type, const DirectXQueueContext* context, const DxObject::BindBufferDesc& desc);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* input assembler *//

	// TODO: culling.

	//* pipeline *//

	std::array<std::unique_ptr<CustomReflectionGraphicsPipeline>, magic_enum::enum_count<LightType>()> pipelines_;

	static inline const std::filesystem::path kDirectory = kPackagesDirectory / "shaders" / "render" / "light";

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreatePipeline();

};

SXAVENGER_ENGINE_NAMESPACE_END
