#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* debug primitive
#include "BaseDebugPrimitive.h"

//* engine
#include <Engine/Foundation.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// DebugPrimitiveLine class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief デバッグ用の線分プリミティブの頂点を蓄積・描画するクラス
class DebugPrimitiveLine
	: public BaseDebugPrimitive {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================
	
	DebugPrimitiveLine();
	~DebugPrimitiveLine() = default;

	void PushLine(const Vector3f& v1, const Vector3f& v2, const Color4f& color, float thickness = 0.0f);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	static const uint32_t kMaxLineNum = (1 << 10);
	static const uint32_t kVertexNum  = 2;

};

SXAVENGER_ENGINE_NAMESPACE_END
