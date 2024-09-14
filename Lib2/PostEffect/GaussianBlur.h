#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// DxObject
#include <DxBufferResource.h>
#include <DxShaderBLob.h>
#include <DxGraphicsPipeline.h>

// c++
#include <memory>

// DrawMethods
#include <DrawMethod.h>

// engine
#include <Texture.h>

////////////////////////////////////////////////////////////////////////////////////////////
// GaussianBlur class
////////////////////////////////////////////////////////////////////////////////////////////
class GaussianBlur {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	GaussianBlur(DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB) { Init(format); }

	~GaussianBlur() { Term(); }

	void Init(DXGI_FORMAT format);

	void Term();

	void CreateBlurTexture(
		BaseTexture* texture, Vector2ui textureSize, Vector2ui strength, float sigma
	);

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// BlurParameter structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct BlurParameter {
		Vector2ui textureSize;
		Vector2ui strength;
		float sigma;
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	// Graphics
	std::unique_ptr<DxObject::GraphicsBlob>     blob_;
	std::unique_ptr<DxObject::GraphicsPipeline> pipeline_;

	// IA
	DrawData sprite_;
	//!< hack: ここまではクラスでの共有値として使用する

	// Buffers
	std::unique_ptr<DxObject::BufferResource<BlurParameter>> param_;

};