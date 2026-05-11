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

//* lib
#include <Lib/Math/Vector2.h>

//* external
#include <magic_enum.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCoreDirectLight class
////////////////////////////////////////////////////////////////////////////////////////////
class FRenderCoreDirectLight final
	: public FBaseRenderCore {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Type enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Type : uint8_t {
		//!< Unlit
		Unlit,

		//!< Punctual light
		Directional,
		Point,
		Spot,

		//!< Rect light
		Rect,

		//!< Sky light
		SkyLight,
		SkyLightEnvironment,
		SkyAtmosphereEnvironment,
		SkyAtmosphereAerial,
	};
	static inline const size_t kTypeCount = magic_enum::enum_count<Type>();

	////////////////////////////////////////////////////////////////////////////////////////////
	// Desc enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Desc : uint8_t {
		Front, //!< Depthが1.0以外に書き込み. [Depth < 1.0]
		Back,  //!< Depthが1.0に書き込み. [Depth == 1.0]
		Aerial,
	};
	static inline const size_t kDescCount = magic_enum::enum_count<Desc>();

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init() override;

	//* pipeline option *//

	void SetPipeline(Type type, const DirectXQueueContext* context, const Vector2ui& resolution) const;

	void BindGraphicsBuffer(Type type, const DirectXQueueContext* context, const DxObject::BindBufferDesc& desc) const;

	//* input assembler option *//

	void DrawCall(const DirectXQueueContext* context, UINT instanceCount = 1) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	// TODO: Lightごとのカリング.
	// TODO: Shader Script への移行.

	//* graphics pipeline *//

	std::array<CustomReflectionGraphicsPipeline, kTypeCount> pipelines_ = {};
	std::array<DxObject::GraphicsPipelineDesc, kDescCount> descs_ = {};

	//* directory *//

	static inline const std::filesystem::path kDirectory = kPackagesDirectory / "shaders" / "render" / "Light";

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateDesc();

	void CreatePipeline();

	//* helper methods *//

	void CreatePipeline(
		Type type, Desc desc,
		std::filesystem::path vertex, std::filesystem::path pixel
	);

};

SXAVENGER_ENGINE_NAMESPACE_END
