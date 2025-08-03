#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>
#include <Engine/Module/Pipeline/CustomGraphicsPipeline.h>

//* lib
#include <Lib/Geometry/Vector2.h>

//* external
#include <magic_enum.hpp>

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
		Forward_CPUParticleVS,
		Forward_GPUParticleVS,
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FRenderCoreGeometry() = default;
	~FRenderCoreGeometry() = default;

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

	DxObject::GraphicsPipelineDesc defferedDesc_ = {};
	DxObject::GraphicsPipelineDesc forwardDesc_  = {};

	//* directory *//

	static inline const std::filesystem::path kDirectory_ = kPackagesShaderDirectory / "render";

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateDesc();

	void CreatePipeline();

};
