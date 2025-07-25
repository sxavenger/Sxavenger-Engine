#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* common
#include "FBaseTexture.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxObjectCommon.h>

//* lib
#include <Lib/Geometry/Vector2.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FTexture class
////////////////////////////////////////////////////////////////////////////////////////////
class FTexture
	: public FBaseTexture {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FTexture() = default;

	void Create(const Vector2ui& size, DXGI_FORMAT format = DxObject::kDefaultOffscreenFormat);

	//* getter *//

	const Vector2ui& GetSize() const { return size_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Vector2ui size_;

};
