#include "DebugPrimitivePoint.h"
SXAVENGER_ENGINE_USING

////////////////////////////////////////////////////////////////////////////////////////////
// DebugPrimitivePoint class methods
////////////////////////////////////////////////////////////////////////////////////////////

DebugPrimitivePoint::DebugPrimitivePoint() {
	BaseDebugPrimitive::CreateInputBuffer(kMaxPointNum);
}

void DebugPrimitivePoint::PushPoint(const Vector3f& v, const Color4f& color, float thickness) {
	BaseDebugPrimitive::SetVertexBuffer(v, color, thickness);
}
