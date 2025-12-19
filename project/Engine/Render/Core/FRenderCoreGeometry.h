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
		Forward_MeshVS,
		Deferred_MeshVS,
		Deferred_MeshMS,
		Forward_CPUParticleVS,
		Forward_GPUParticleVS,
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

	DxObject::GraphicsPipelineDesc deferredDesc_ = {};
	DxObject::GraphicsPipelineDesc forwardDesc_  = {};

	//* directory *//

	static inline const std::filesystem::path kDirectory = kPackagesDirectory / "shaders" / "render";

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateDesc();

	void CreatePipeline();

};

SXAVENGER_ENGINE_NAMESPACE_END
