#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// Geometry
#include <Lib/Geometry/Vector2.h>
#include <Lib/Geometry/Vector3.h>
#include <Lib/Geometry/Vector4.h>

////////////////////////////////////////////////////////////////////////////////////////////
// structure
////////////////////////////////////////////////////////////////////////////////////////////

struct MeshVertexData {
	Vector4f position;
	Vector2f texcoord;
	Vector3f normal;

	//!< for tangent space
	Vector3f tangent;
	Vector3f bitangent;

	void Init() {
		position = { 0.0f, 0.0f, 0.0f, 1.0f };
		texcoord = { 0.0f, 0.0f };
		normal   = { 0.0f, 0.0f, 0.0f };

		tangent   = { 0.0f, 0.0f, 0.0f };
		bitangent = { 0.0f, 0.0f, 0.0f };
	}
};
