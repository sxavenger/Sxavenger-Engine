#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* input geometry
#include "InputPrimitive.h"

//* engine
#include <Engine/Foundation.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// InputPrimitiveHelper class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief InputPrimitive生成補助クラス
class InputPrimitiveHelper {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static InputPrimitive CreatePlane(const Vector2f& size); //!< 廃止予定

	static InputPrimitive CreatePlaneZForward(const Vector2f& size);

	static InputPrimitive CreateCube(const Vector3f& size);

};

SXAVENGER_ENGINE_NAMESPACE_END
