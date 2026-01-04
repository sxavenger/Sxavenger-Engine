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
// CanvasVertexData structure
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief キャンバス頂点データ構造体.
struct CanvasVertexData {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init();

	//=========================================================================================
	// public variables
	//=========================================================================================

	Vector3f position = kOrigin3<float>;
	Vector2f texcoord = kOrigin2<float>;
	Color4f  color    = kWhite4<float>;

};

////////////////////////////////////////////////////////////////////////////////////////////
// InputCanvasVertex class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief CanvasVertexData用のInputAssemblerクラス.
class InputCanvasVertex
	: public TriangleInputAssembler<CanvasVertexData> {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	InputCanvasVertex()  = default;
	~InputCanvasVertex() = default;

	DELETE_COPY(InputCanvasVertex)
	DEFAULT_MOVE(InputCanvasVertex)

private:
};

SXAVENGER_ENGINE_NAMESPACE_END
