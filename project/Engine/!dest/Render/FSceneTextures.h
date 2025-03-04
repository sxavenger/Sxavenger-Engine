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
		Material_AO,
		Albedo,
		Position, // todo: depthから復元する
		Main,
	};
	static const uint8_t kGBufferLayoutCount = static_cast<uint8_t>(GBufferLayout::Main) + 1;

	//* [detail GBuffer Layout]
	//* Normal:    float3 normal, float _NOT_USED_1                          [R8G8B8A8_UNORM]
	//* Material:  float metallic, float specular, float roughness, float AO [R8G8B8A8_UNORM]
	//* Albedo_AO: float3 albedo, float _NOT_USED_1                          [R32G32B32A32_FLOAT] //!< HDRのため32bit浮動小数点
	//* Position:  float3 position, float _NOT_USED_1                        [R32G32B32A32_FLOAT] //!< HDRのため32bit浮動小数点
	//* Main:      float4 color                                              [R32G32B32A32_FLOAT] //!< HDRのため32bit浮動小数点

	//* [define detail]
	//* _NOT_USED_1: 未使用のデータ(初期値1.0f)

	// todo: AOをMaterialに移動

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FSceneTextures()  = default;
	~FSceneTextures() = default;

	void Create(const Vector2ui& size);

	//* option *//

	void BeginOpaqueBasePass(const DirectXThreadContext* context) const;
	void EndOpaqueBasePass(const DirectXThreadContext* context) const;

	void BeginLightingPass(const DirectXThreadContext* context) const;
	void EndLightingPass(const DirectXThreadContext* context) const;

	void BeginTransparentBasePass(const DirectXThreadContext* context) const;
	void EndTransparentBasePass(const DirectXThreadContext* context) const;

	void BeginPostProcessPass(const DirectXThreadContext* context) const;
	void EndPostProcessPass(const DirectXThreadContext* context) const;

	void BeginCanvasPass(const DirectXThreadContext* context) const;
	void EndCanvasPass(const DirectXThreadContext* context) const;

	void BeginForward(const DirectXThreadContext* context) const;
	void EndForward(const DirectXThreadContext* context) const;

	//* getter *//

	const Vector2ui& GetSize() const { return parameterBuffer_->At(0).size; }

	const FTexture* GetGBuffer(GBufferLayout layout) const { return gBuffers_[static_cast<uint8_t>(layout)].get(); }

	const FSceneDepth* GetDepth() const { return depth_.get(); }

	const D3D12_GPU_VIRTUAL_ADDRESS GetParameter() const { return parameterBuffer_->GetGPUVirtualAddress(); }

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
