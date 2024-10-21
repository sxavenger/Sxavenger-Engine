#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Texture.h>

//* c++
#include <array>
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// SceneRenderTarget class
////////////////////////////////////////////////////////////////////////////////////////////
class SceneRenderTarget {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// GBufferType enum
	////////////////////////////////////////////////////////////////////////////////////////////
	enum GBuffer : uint32_t {
		GBUFFER_ALBEDO,
		GBUFFER_NORMAL,
		GBUFFER_POSITION,

		/*GBUFFER_GLOSSINESS,
		GBUFFER_ATMOSPHERE,*/

		kCountOfGBuffer
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	SceneRenderTarget() = default;
	~SceneRenderTarget() { Term(); }

	void Create(const Vector2ui& size);

	void Term();

	//* getter *//
	//* GBuffer
	const std::array<std::unique_ptr<RenderTexture>, kCountOfGBuffer>& GetGBuffers() const { return gBuffers_; }

	//* DepthStencil
	const D3D12_CPU_DESCRIPTOR_HANDLE& GetCPUHandleDSV() const { return descriptorDSV_.GetCPUHandle(); }

private:

	//=========================================================================================
	// private varaibles
	//=========================================================================================

	//* GBuffers *//

	std::array<DXGI_FORMAT,                    kCountOfGBuffer> formats_;
	std::array<std::unique_ptr<RenderTexture>, kCountOfGBuffer> gBuffers_;

	//* depthStencil *//

	ComPtr<ID3D12Resource> resourceDSV_;
	DxObject::Descriptor descriptorDSV_;
	// todo: DSVがTextureとして使えるように変更

	//* parameter *//

	Vector2ui size_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateGBuffer();
	void CreateDepthStencil();

};