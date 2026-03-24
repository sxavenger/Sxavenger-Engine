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
#include <Lib/Geometry/Vector2.h>

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
// FRenderCoreGeometry class
////////////////////////////////////////////////////////////////////////////////////////////
class FRenderCoreGeometry final
	: public FBaseRenderCore {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Pipeline enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Pipeline : uint32_t {
		ForwardPrepass_MeshVS,
		ForwardPrepass_MeshMS,
		ForwardTransparent_MeshVS,
		ForwardTransparent_MeshMS,
		Deferred_MeshVS,
		Deferred_MeshMS,
	};
	static inline const size_t kPipelineCount = magic_enum::enum_count<Pipeline>();

	////////////////////////////////////////////////////////////////////////////////////////////
	// Desc enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Desc : uint8_t {
		Deferred,
		ForwardPrepass,
		ForwardTransparent,
	};
	static inline const size_t kDescCount = magic_enum::enum_count<Desc>();

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init() override;

	//* pipeline option *//

	void SetPipeline(Pipeline pipeline, const DirectXQueueContext* context, const Vector2ui& resolution) const;

	void BindGraphicsBuffer(Pipeline pipeline, const DirectXQueueContext* context, const DxObject::BindBufferDesc& desc) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* graphics pipeline *//

	std::array<CustomReflectionGraphicsPipeline, kPipelineCount> pipelines_;
	std::array<DxObject::GraphicsPipelineDesc, kDescCount> descs_ = {};

	// TODO: Shader Script への移行

	//* directory *//

	static inline const std::filesystem::path kDirectory = kPackagesDirectory / "shaders" / "render" / "Geometry";

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateDesc();

	void CreatePipeline();

};

SXAVENGER_ENGINE_NAMESPACE_END
