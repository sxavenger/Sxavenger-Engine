#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* debug primitive
#include "DebugPrimitivePoint.h"
#include "DebugPrimitiveLine.h"

//* engine
#include <Engine/Foundation.h>

//* lib
#include <Lib/Geometry/Matrix4x4.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// DebugPrimitive class
////////////////////////////////////////////////////////////////////////////////////////////
class DebugPrimitive {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init();

	void Term();

	void DrawToScene(const DirectXQueueContext* context, const D3D12_GPU_VIRTUAL_ADDRESS& camera);

	void ResetPrimitive();

	void PushLine(const Vector3f& v1, const Vector3f& v2, const Color4f& color, float thickness = 0.0f);

	void PushLineOverlay(const Vector3f& v1, const Vector3f& v2, const Color4f& color, float thickness = 0.0f);

	void PushPoint(const Vector3f& v, const Color4f& color, float thickness = 0.0f);

	void PushPointOverlay(const Vector3f& v, const Color4f& color, float thickness = 0.0f);

	//* drawer options *//

	void PushGrid(const Vector3f& center, float size);
	void PushGrid(const Matrix4x4& viewInv, const Matrix4x4& projInv, const Vector2f& size, float radius);

	void PushAxis(const Vector3f& center, float length);

	void PushBox(const Vector3f& min, const Vector3f& max, const Color4f& color, float thickness = 0.0f);
	void PushBox(const Vector3f& position, const Vector3f& min, const Vector3f& max, const Color4f& color, float thickness = 0.0f);

	void PushCube(const Vector3f& center, const Vector3f& size, const Color4f& color, float thickness = 0.0f);

	void PushSphere(const Vector3f& center, float radius, const Color4f& color);

	void PushCone(const Vector3f& center, const Vector3f& direction, float radius, float angle, const Color4f& color);

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// PipelineType enum
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class PipelineType : uint32_t {
		Line,
		LineOverlay,
		Point,
		PointOverlay
	};

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* primitive *//

	std::unique_ptr<DebugPrimitiveLine> line_;
	std::unique_ptr<DebugPrimitiveLine> lineOverlay_;
	std::unique_ptr<DebugPrimitivePoint> point_;
	std::unique_ptr<DebugPrimitivePoint> pointOverlay_;

	//* pipeline *//

	std::array<std::unique_ptr<DxObject::ReflectionGraphicsPipelineState>, magic_enum::enum_count<PipelineType>()> pipelines_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreatePrimitive();

	void CreatePipeline();

};

SXAVENGER_ENGINE_NAMESPACE_END
