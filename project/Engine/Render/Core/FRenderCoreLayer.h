#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Module/Pipeline/CustomGraphicsPipeline.h>

//* external
#include <magic_enum.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCoreLight class
////////////////////////////////////////////////////////////////////////////////////////////
class FRenderCoreLayer {
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

	FRenderCoreLayer()  = default;
	~FRenderCoreLayer() = default;

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

	static const std::filesystem::path kDirectory_;

	//=========================================================================================
	// private variables
	//=========================================================================================

	void CreateDesc();

	void CreatePipeline();

};
