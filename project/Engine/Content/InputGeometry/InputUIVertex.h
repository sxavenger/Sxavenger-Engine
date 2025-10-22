#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* content
#include "../InputAssembler/TriangleInputAssembler.h"

//* lib
#include <Lib/Traits.h>
#include <Lib/Geometry/Vector2.h>
#include <Lib/Geometry/Vector3.h>
#include <Lib/Geometry/Color4.h>

////////////////////////////////////////////////////////////////////////////////////////////
// UIVertexData structure
////////////////////////////////////////////////////////////////////////////////////////////
struct UIVertexData {
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
	Color4f  color    = { 0, 0, 0, 0 };

};

////////////////////////////////////////////////////////////////////////////////////////////
// InputUIVertex class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief UIVertexData用のInputAssemblerクラス.
class InputUIVertex
	: public TriangleInputAssembler<UIVertexData> {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	InputUIVertex()  = default;
	~InputUIVertex() = default;

	_DELETE_COPY(InputUIVertex)
	_DEFAULT_MOVE(InputUIVertex)

};

