#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// DxObject
#include <DxBufferResource.h>

// engine
#include <TextureManager.h>

// Geometry
#include <Vector2.h>
#include <ObjectStructure.h>

// memory
#include <memory>

#include <Attribute.h>

////////////////////////////////////////////////////////////////////////////////////////////
// GaussianBlur class
////////////////////////////////////////////////////////////////////////////////////////////
class GaussianBlur
	: public Attribute {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	GaussianBlur() { Init(); }

	~GaussianBlur() { Term(); }

	void Init();

	void Term();

	void CreateBlurTexture(
		int32_t width, int32_t height, const D3D12_GPU_DESCRIPTOR_HANDLE& texture
	);

	void CreateBlurTexture(
		Texture* outputTexture,
		int32_t width, int32_t height, const D3D12_GPU_DESCRIPTOR_HANDLE& texture
	);

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetTexture() {
		return outputTexture_->GetSRVHandleGPU();
	}

	void SetAttributeImGui() override;

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// BlurParameter structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct BlurParameter {
		Vector2f renderSize;    // renderTextureサイズ
		int isHolizontalEnable; // x軸方向へのブラーが有効か
		int isVerticlaEnable;   // y軸方向へのブラーが有効か
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	// IA
	std::unique_ptr<DxObject::BufferResource<VertexData>> vertex_;
	std::unique_ptr<DxObject::IndexBufferResource>        index_;

	// constantBuffer
	std::unique_ptr<DxObject::BufferResource<Matrix4x4>>     matrix_;
	std::unique_ptr<DxObject::BufferResource<BlurParameter>> param_;

	std::unique_ptr<RenderTexture> outputTexture_;


};