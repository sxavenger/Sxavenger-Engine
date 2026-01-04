#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* graphics
#include "../InputAssembler/TriangleInputAssembler.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>
#include <Engine/System/DirectX/DxObject/DxBindBuffer.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

//* lib
#include <Lib/Traits.h>
#include <Lib/Geometry/Vector2.h>
#include <Lib/Geometry/Vector3.h>
#include <Lib/Geometry/Vector4.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// PrimitiveVertexData structure
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief プリミティブ頂点データ構造体
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
//! @brief プリミティブInputAssemblerクラス
class InputPrimitive
	: public TriangleInputAssembler<PrimitiveVertexData> {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	InputPrimitive()  = default;
	~InputPrimitive() = default;

	DELETE_COPY(InputPrimitive)
	DEFAULT_MOVE(InputPrimitive)

private:
};

SXAVENGER_ENGINE_NAMESPACE_END
