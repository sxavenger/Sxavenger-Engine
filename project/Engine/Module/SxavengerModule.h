#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "Primitive/DebugPrimitive.h"

////////////////////////////////////////////////////////////////////////////////////////////
// SxavengerModule class
////////////////////////////////////////////////////////////////////////////////////////////
class SxavengerModule {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static void Init();

	static void Term();

	//-----------------------------------------------------------------------------------------
	// primitive option
	//-----------------------------------------------------------------------------------------

	static void ResetPrimtive();

	static void DrawToScene(const DirectXThreadContext* context, const Camera3d* camera);

	static void DrawLine(const Vector3f& v1, const Vector3f& v2, const Color4f& color);

	static void DrawAxis(const Vector3f& center, float length);

	static DebugPrimitive* GetDebugPrimitive();

private:
};