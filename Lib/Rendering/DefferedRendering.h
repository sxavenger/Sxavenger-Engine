#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// Texture
#include <TextureManager.h>

// DxObject
#include <DxBufferResource.h>
#include <DxGraphicsBlob.h>
#include <DxGraphicsPipeline.h>

#include <ObjectStructure.h>

// c++
#include <array>
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// DefferedRenderingType enum
////////////////////////////////////////////////////////////////////////////////////////////
enum DefferedRenderingType {
	DEFFERERD_ALBED,
	DEFFERERD_DEPTH,
	DEFFERERD_NORMAL,
	DEFFERERD_WORLD,

	kCountOfDefferedRenderingType
};

static_assert(DefferedRenderingType::kCountOfDefferedRenderingType <= D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT);
//!< 1度に書き込めるRTV数の限界

////////////////////////////////////////////////////////////////////////////////////////////
// DefferedRendering class
////////////////////////////////////////////////////////////////////////////////////////////
class DefferedRendering {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	DefferedRendering() { Init(); }

	~DefferedRendering() { Term(); }

	void Init();

	void Draw(const D3D12_GPU_VIRTUAL_ADDRESS& cameraAddress, const D3D12_GPU_VIRTUAL_ADDRESS& lightAddress);

	void Term();

	static const DXGI_FORMAT* GetFormats() { return formats_.data(); }

	RenderTexture** GetTexturePtrs() { return texturePtrs_.data(); }

	D3D12_GPU_DESCRIPTOR_HANDLE* GetTextureHandles() { return textureHandles_.data(); }

private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	static const std::array<DXGI_FORMAT, DefferedRenderingType::kCountOfDefferedRenderingType> formats_;
	
	/* deffered gBuffers */
	std::array<std::unique_ptr<RenderTexture>, DefferedRenderingType::kCountOfDefferedRenderingType> defferedTextures_;

	std::array<RenderTexture*, DefferedRenderingType::kCountOfDefferedRenderingType>              texturePtrs_; //!< 外部参照用
	std::array<D3D12_GPU_DESCRIPTOR_HANDLE, DefferedRenderingType::kCountOfDefferedRenderingType> textureHandles_;

	/* graphics rendering */
	// IA
	std::unique_ptr<DxObject::BufferResource<VertexData>> vertexBuffer_;
	std::unique_ptr<DxObject::IndexBufferResource>        indexBuffer_;

	std::unique_ptr<DxObject::GraphicsBlob>     blob_;
	std::unique_ptr<DxObject::GraphicsPipeline> pipeline_;

};