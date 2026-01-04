#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Foundation.h>
#include <Engine/System/Configuration/Configuration.h>
#include <Engine/Module/Pipeline/CustomGraphicsPipeline.h>

//* external
#include <magic_enum.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCoreCanvas class
////////////////////////////////////////////////////////////////////////////////////////////
class FRenderCoreCanvas {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Type enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Type : uint8_t {
		Sprite,
		Text
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init();

	//* pipeline option *//

	void SetPipeline(Type type, const DirectXQueueContext* context, const Vector2ui& size) const;

	void BindGraphicsBuffer(Type type, const DirectXQueueContext* context, const DxObject::BindBufferDesc& desc) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	DxObject::GraphicsPipelineDesc desc_;

	std::array<std::unique_ptr<CustomReflectionGraphicsPipeline>, magic_enum::enum_count<Type>()> pipelines_;

	static inline const std::filesystem::path kDirectory = kPackagesDirectory / "shaders" / "render" / "canvas";

	//=========================================================================================
	// private variables
	//=========================================================================================

	void CreateDesc();

	void CreatePipeline();

};

SXAVENGER_ENGINE_NAMESPACE_END

