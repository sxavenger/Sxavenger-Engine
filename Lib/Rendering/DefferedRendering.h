#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// Texture
#include <TextureManager.h>

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

	void Term();

	static const DXGI_FORMAT* GetFormats() { return formats_; }

	RenderTexture** GetPtrs() { return texturePtrs_.data(); }

private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	static const DXGI_FORMAT formats_[DefferedRenderingType::kCountOfDefferedRenderingType];
	
	std::array<std::unique_ptr<RenderTexture>, DefferedRenderingType::kCountOfDefferedRenderingType> defferedTextures_;
	std::array<RenderTexture*, DefferedRenderingType::kCountOfDefferedRenderingType>                 texturePtrs_; //!< 外部参照用

};