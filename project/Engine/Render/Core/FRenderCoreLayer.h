#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Module/Pipeline/CustomGraphicsPipeline.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCoreLight class
////////////////////////////////////////////////////////////////////////////////////////////
class FRenderCoreLayer {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// PipelineType enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class PipelineType : uint8_t {
		Sprite,
	};
	static const uint8_t kPipelineTypeCount = static_cast<uint8_t>(PipelineType::Sprite) + 1;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FRenderCoreLayer()  = default;
	~FRenderCoreLayer() = default;

	void Init();

	//* pipeline option *//

	void SetPipeline(PipelineType type, const DirectXQueueContext* context, const Vector2ui& size) const;

	void BindGraphicsBuffer(PipelineType type, const DirectXQueueContext* context, const DxObject::BindBufferDesc& desc) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	DxObject::GraphicsPipelineDesc desc_;

	std::array<std::unique_ptr<CustomReflectionGraphicsPipeline>, kPipelineTypeCount> pipelines_;

	static const std::filesystem::path kDirectory_;

	//=========================================================================================
	// private variables
	//=========================================================================================

	void CreateDesc();

	void CreatePipeline();

};
