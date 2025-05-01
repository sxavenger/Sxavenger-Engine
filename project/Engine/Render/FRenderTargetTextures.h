#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "Common/FTexture.h"
#include "Common/FDepthTexture.h"
#include "Common/FProcessTexture.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderTargetTextures class
////////////////////////////////////////////////////////////////////////////////////////////
class FRenderTargetTextures {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// GBufferLayout enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class GBufferLayout : uint8_t {
		Normal,
		Material_AO,
		Albedo,
		Position, // todo: depthから復元する
		UI,
		Main,
	};
	static const uint8_t kGBufferLayoutCount = static_cast<uint8_t>(GBufferLayout::Main) + 1;

	//* [define detail]
	//* _NOT_USED_1: 未使用のデータ(初期値1.0f)
	
	//* [detail GBuffer Layout]
	//* Normal:    float3 normal, float _NOT_USED_1                          [DXGI_FORMAT_R10G10B10A2_UNORM]
	//* Material:  float metallic, float specular, float roughness, float AO [R8G8B8A8_UNORM]
	//* Albedo_AO: float3 albedo, float _NOT_USED_1                          [R32G32B32A32_FLOAT] //!< HDRのため32bit浮動小数点
	//* Position:  float3 position, float _NOT_USED_1                        [R32G32B32A32_FLOAT] //!< HDRのため32bit浮動小数点
	//* UI: spriteでの使用
	//* Main:      float4 color                                              [R32G32B32A32_FLOAT] //!< HDRのため32bit浮動小数点

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FRenderTargetTextures()  = default;
	~FRenderTargetTextures() = default;

	void Create(const Vector2ui& size);

	//* clear option *//

	void ClearTextures(const DirectXThreadContext* context) const;

	//* deferred option *//

	void BeginGeometryPass(const DirectXThreadContext* context) const;

	void EndGeometryPass(const DirectXThreadContext* context) const;

	void SetupGeometryPass(const DirectXThreadContext* context) const;

	void BeginLightingPass(const DirectXThreadContext* context) const;

	void EndLightingPass(const DirectXThreadContext* context) const;

	void BeginTransparentBasePass(const DirectXThreadContext* context) const;

	void EndTransparentBasePass(const DirectXThreadContext* context) const;

	void BeginCanvasPass(const DirectXThreadContext* context) const;

	void EndCanvasPass(const DirectXThreadContext* context) const;

	//* process option *//

	void BeginPostProcess(const DirectXThreadContext* context);

	void EndPostProcess(const DirectXThreadContext* context);

	//* getter *//

	const Vector2ui& GetSize() const { return size_; }

	const FTexture* GetGBuffer(GBufferLayout layout) const { return gBuffers_[static_cast<size_t>(layout)].get(); }

	const FDepthTexture* GetDepth() const { return depth_.get(); }

	FProcessTextures* GetProcessTextures() { return process_.get(); }

	const D3D12_GPU_VIRTUAL_ADDRESS& GetDimension() const { return dimension_->GetGPUVirtualAddress(); }

	static const DXGI_FORMAT GetFormat(GBufferLayout layout) { return kGBufferFormats_[static_cast<size_t>(layout)]; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* formats *//

	static const std::array<DXGI_FORMAT, kGBufferLayoutCount> kGBufferFormats_;

	//* GBuffer textures *//

	std::array<std::unique_ptr<FTexture>, kGBufferLayoutCount> gBuffers_;

	//* depth textures *//

	std::unique_ptr<FDepthTexture> depth_;

	//* process texture *//
	std::unique_ptr<FProcessTextures> process_;

	//* parameter *//

	Vector2ui size_;
	//!< gpu parameter buffer は GetDimension()関数で取得する

	std::unique_ptr<DxObject::DimensionBuffer<Vector2ui>> dimension_;

};
