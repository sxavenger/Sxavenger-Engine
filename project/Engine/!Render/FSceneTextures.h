#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* renderer
#include "FSceneDepth.h"
#include "FTexture.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>

//* c++
#include <cstdint>

////////////////////////////////////////////////////////////////////////////////////////////
// FSceneTextures class
////////////////////////////////////////////////////////////////////////////////////////////
class FSceneTextures { //!< GBuffer管理クラス
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// GBufferLayout enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class GBufferLayout : uint8_t {
		Normal,
		Material,
		Albedo_AO,

		// HACK: Deferredが完成したら削除するkamo
		Position,
	};
	static const uint8_t kGBufferLayoutCount = static_cast<uint8_t>(GBufferLayout::Position) + 1;

	//* [detail GBuffer Layout]
	//* Normal:    float3 normal, float _NOT_USED_1                                   [R8G8B8A8_UNORM]
	//* Material:  float metallic, float specular, float roughness, float _NOT_USED_1 [R8G8B8A8_UNORM]
	//* Albedo_AO: float3 albedo, float AO, float _NOT_USED_1                         [R32G32B32A32_FLOAT] //!< HDRのため32bit浮動小数点
	//* Position:  float3 position, float _NOT_USED_1                                 [R32G32B32A32_FLOAT] //!< HDRのため32bit浮動小数点

	//* [define detail]
	//* _NOT_USED_1: 未使用のデータ(初期値1.0f)

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FSceneTextures()  = default;
	~FSceneTextures() = default;

	void Create(const Vector2ui& size);

	//* option *//

	void BeginBasePass(const DirectXThreadContext* context) const;

	void EndBasePass(const DirectXThreadContext* context) const;

	//* getter *//

	const Vector2ui& GetSize() const { return parameterBuffer_->At(0).size; }

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// TexturesParameter structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct TexturesParameter {
		//* member *//
		Vector2ui size;
	};

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* format *//

	static const std::array<DXGI_FORMAT, kGBufferLayoutCount> kGBufferFormats;

	//* GBuffer textures *//

	std::array<std::unique_ptr<FTexture>, kGBufferLayoutCount> gBuffers_;

	//* depth textures *//

	std::unique_ptr<FSceneDepth> depth_;

	//* parameter buffer *//

	std::unique_ptr<DxObject::DimensionBuffer<TexturesParameter>> parameterBuffer_;

};
