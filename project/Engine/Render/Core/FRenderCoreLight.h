#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Content/InputAssembler/TriangleInputAssembler.h>
#include <Engine/Module/Pipeline/CustomGraphicsPipeline.h>

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
		AlbedoEmpty,
		Directional,
		Point,
		Spot,
		Rect,
		SkyLight,
	};
	static const uint8_t kLightTypeCount = SkyLight + 1;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FRenderCoreLight()  = default;
	~FRenderCoreLight() = default;

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

	std::array<std::unique_ptr<CustomReflectionGraphicsPipeline>, kLightTypeCount> pipelines_;

	static const std::filesystem::path kDirectory_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreatePipeline();

};
