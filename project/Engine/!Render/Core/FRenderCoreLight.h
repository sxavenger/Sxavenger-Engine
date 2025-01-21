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
		Directional,
		Point,
		Spot,
	};
	static const uint8_t kLightTypeCount = Spot + 1;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FRenderCoreLight()  = default;
	~FRenderCoreLight() = default;

	void Init();

	//* input assembler option *//

	void BindIABuffer(const DirectXThreadContext* context) const;

	void DrawCall(const DirectXThreadContext* context) const;

	//* pipeline option *//

	void SetPipeline(LightType type, const DirectXThreadContext* context, const Vector2ui& size);

	void BindGraphicsBuffer(LightType type, const DirectXThreadContext* context, const DxObject::BindBufferDesc& desc);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* input assembler *//

	TriangleInputAssembler<Vector2f> ia_;
	// HACK: 全画面の描画用のIA
	// TODO: culling.

	//* pipeline *//

	std::array<std::unique_ptr<CustomReflectionGraphicsPipeline>, kLightTypeCount> pipelines_;

	static const std::filesystem::path kDirectory_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateInputAssembler();
	void CreatePipeline();

};
