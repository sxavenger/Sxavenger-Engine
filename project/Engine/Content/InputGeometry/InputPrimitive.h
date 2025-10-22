#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* content
#include "../InputAssembler/TriangleInputAssembler.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>
#include <Engine/System/DirectX/DxObject/DxBindBuffer.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

//* lib
#include <Lib/Traits.h>
#include <Lib/Geometry/Vector2.h>
#include <Lib/Geometry/Vector3.h>
#include <Lib/Geometry/Vector4.h>

////////////////////////////////////////////////////////////////////////////////////////////
// PrimitiveVertexData structure
////////////////////////////////////////////////////////////////////////////////////////////
struct PrimitiveVertexData {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init();

	//=========================================================================================
	// public variables
	//=========================================================================================

	Vector4f position;
	Vector2f texcoord;
	Vector3f normal;

};

////////////////////////////////////////////////////////////////////////////////////////////
// InputPrimtive class
////////////////////////////////////////////////////////////////////////////////////////////
class InputPrimitive
	: public TriangleInputAssembler<PrimitiveVertexData> {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	InputPrimitive()  = default;
	~InputPrimitive() = default;

	_DELETE_COPY(InputPrimitive)
	_DEFAULT_MOVE(InputPrimitive)

private:
};
