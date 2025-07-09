#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* input geometry
#include "InputPrimitive.h"

////////////////////////////////////////////////////////////////////////////////////////////
// InputPrimitiveHelper class
////////////////////////////////////////////////////////////////////////////////////////////
class InputPrimitiveHelper {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static InputPrimitive CreatePlane(const Vector2f& size); //!< 廃止予定

	static InputPrimitive CreatePlaneZForward(const Vector2f& size);

	static InputPrimitive CreateCube(const Vector3f& size);

};
