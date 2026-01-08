#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Foundation.h>
#include <Engine/System/Configuration/Configuration.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>
#include <Engine/Module/Pipeline/CustomGraphicsPipeline.h>

//* lib
#include <Lib/Geometry/Vector2.h>

//* external
#include <magic_enum.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCoreGeometry class
////////////////////////////////////////////////////////////////////////////////////////////
class FRenderCoreGeometry {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Type enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Type : uint32_t {
		ForwardOpaque_MeshVS,
		ForwardOpaque_MeshMS,
		ForwardTransparent_MeshVS,
		ForwardTransparent_MeshMS,
		Deferred_MeshVS,
		Deferred_MeshMS,
		//Forward_CPUParticleVS,
		//Forward_GPUParticleVS,
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Desc enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Desc : uint8_t {
		Deferred,
		ForwardOpaque,
		ForwardTransparent,
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init();

	//* pipeline option *//

	void SetPipeline(Type type, const DirectXQueueContext* context, const Vector2ui& size);

	void BindGraphicsBuffer(Type type, const DirectXQueueContext* context, const DxObject::BindBufferDesc& desc);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* graphics pipeline *//

	std::array<std::unique_ptr<CustomReflectionGraphicsPipeline>, magic_enum::enum_count<Type>()> pipelines_;

	std::array<DxObject::GraphicsPipelineDesc, magic_enum::enum_count<Desc>()> descs_ = {};

	//* directory *//

	static inline const std::filesystem::path kDirectory = kPackagesDirectory / "shaders" / "render";

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateDesc();

	void CreatePipeline();

};

SXAVENGER_ENGINE_NAMESPACE_END
