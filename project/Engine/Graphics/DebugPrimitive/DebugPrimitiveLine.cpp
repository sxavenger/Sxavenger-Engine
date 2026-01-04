#include "DebugPrimitiveLine.h"
SXAVENGER_ENGINE_USING

////////////////////////////////////////////////////////////////////////////////////////////
// DebugPrimitiveLine class methods
////////////////////////////////////////////////////////////////////////////////////////////

DebugPrimitiveLine::DebugPrimitiveLine() {
	BaseDebugPrimitive::CreateInputBuffer(kMaxLineNum * kVertexNum);
}

void DebugPrimitiveLine::PushLine(const Vector3f& v1, const Vector3f& v2, const Color4f& color, float thickness) {
	BaseDebugPrimitive::SetVertexBuffer(v1, color, thickness);
	BaseDebugPrimitive::SetVertexBuffer(v2, color, thickness);
}
