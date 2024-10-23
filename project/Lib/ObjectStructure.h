#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// Geometry
#include <Lib/Geometry/Vector2.h>
#include <Lib/Geometry/Vector3.h>
#include <Lib/Geometry/Vector4.h>
#include <Lib/Geometry/Quaternion.h>
#include <Lib/Geometry/Matrix4x4.h>

// imgui
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// structure
////////////////////////////////////////////////////////////////////////////////////////////

struct VertexData {
	Vector4f position;
	Vector2f texcoord;
	Vector3f normal;

	void Init() {
		position = { 0.0f, 0.0f, 0.0f, 1.0f };
		texcoord = { 0.0f, 0.0f };
		normal   = { 0.0f, 0.0f, 0.0f };
	}
};