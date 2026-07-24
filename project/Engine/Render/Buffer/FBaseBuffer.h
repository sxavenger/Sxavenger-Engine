#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/DxObject/DxObjectCommon.h>

//* c++
#include <concepts>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// FBaseBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
class FBaseBuffer {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FBaseBuffer()          = default;
	virtual ~FBaseBuffer() = default;

	virtual void Create(const Vector2ui& resolution) = 0;

	void Resize(const Vector2ui& resolution);

	//=========================================================================================
	// public variables
	//=========================================================================================

	static const DXGI_FORMAT kColorFormat        = DXGI_FORMAT_R16G16B16A16_FLOAT;
	static const DXGI_FORMAT kDepthStencilFormat = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Vector2ui resolution_ = {}; //!< 内部バッファの解像度

};

SXAVENGER_ENGINE_NAMESPACE_END
