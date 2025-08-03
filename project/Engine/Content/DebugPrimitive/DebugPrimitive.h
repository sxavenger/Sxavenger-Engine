#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>
#include <Engine/System/DirectX/DxObject/DxGraphicsPipelineState.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

//* lib
#include <Lib/Geometry/Vector4.h>
#include <Lib/Geometry/Vector3.h>
#include <Lib/Geometry/Color4.h>

//* external
#include <magic_enum.hpp>

//* c++
#include <memory>

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class CameraComponent;

////////////////////////////////////////////////////////////////////////////////////////////
// BaseDebugPrimitive class
////////////////////////////////////////////////////////////////////////////////////////////
class BaseDebugPrimitive {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	BaseDebugPrimitive()          = default;
	virtual ~BaseDebugPrimitive() = default;

	void Draw(const DirectXQueueContext* context);

	void Reset();

	void CountBufferOffset();

protected:

	////////////////////////////////////////////////////////////////////////////////////////////
	// PrimitiveInput structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct PrimitiveInput {
		Vector4f position;
		Color4f  color;
		float thickness;
	};

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* buffer *//

	std::unique_ptr<DxObject::VertexDimensionBuffer<PrimitiveInput>> input_;

	//* member *//

	uint32_t inputCount_  = 0; //!< 描画数
	uint32_t inputOffset_ = 0; //!< 前sceneに描画した分

	//=========================================================================================
	// protected methods
	//=========================================================================================

	void CreateInputBuffer(uint32_t size);

	void SetVertexBuffer(const Vector3f& position, const Color4f& color, float thickness);

};

////////////////////////////////////////////////////////////////////////////////////////////
// DebugPrimitivePoint class
////////////////////////////////////////////////////////////////////////////////////////////
class DebugPrimitivePoint
	: public BaseDebugPrimitive {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	DebugPrimitivePoint() { Init(); }
	~DebugPrimitivePoint() = default;

	void Init();

	void PushPoint(const Vector3f& v, const Color4f& color, float thickness = 0.0f);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* config *//

	static const uint32_t kMaxPointNum_ = (1 << 10);

};

////////////////////////////////////////////////////////////////////////////////////////////
// DebugPrimitiveLine class
////////////////////////////////////////////////////////////////////////////////////////////
class DebugPrimitiveLine
	: public BaseDebugPrimitive {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	DebugPrimitiveLine() { Init(); }
	~DebugPrimitiveLine() = default;

	void Init();

	void PushLine(const Vector3f& v1, const Vector3f& v2, const Color4f& color, float thickness = 0.0f);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* config *//

	static const uint32_t kMaxLineNum_ = (1 << 16);
	static const uint32_t kVertexNum_  = 2; //!< lineなので2頂点

};

////////////////////////////////////////////////////////////////////////////////////////////
// DebugPrimitive class
////////////////////////////////////////////////////////////////////////////////////////////
class DebugPrimitive {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	DebugPrimitive() = default;
	~DebugPrimitive() = default;

	void Init();

	void Term();

	void DrawToScene(const DirectXQueueContext* context, const CameraComponent* camera);

	void ResetPrimitive();

	void PushLine(const Vector3f& v1, const Vector3f& v2, const Color4f& color, float thickness = 0.0f);

	void PushLineOverlay(const Vector3f& v1, const Vector3f& v2, const Color4f& color, float thickness = 0.0f);

	void PushPoint(const Vector3f& v, const Color4f& color, float thickness = 0.0f);

	void PushPointOverlay(const Vector3f& v, const Color4f& color, float thickness = 0.0f);

	//* drawer options *//

	void PushGrid(const Vector3f& center, float size);
	void PushGrid(const CameraComponent* camera, const Vector2f& size, float radius);

	void PushAxis(const Vector3f& center, float length);

	void PushBox(const Vector3f& min, const Vector3f& max, const Color4f& color);
	void PushCube(const Vector3f& center, const Vector3f& size, const Color4f& color);

	void PushSphere(const Vector3f& center, float radius, const Color4f& color);

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
	static inline const size_t kPipelineCount = magic_enum::enum_count<PipelineType>();

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

	std::array<std::unique_ptr<DxObject::ReflectionGraphicsPipelineState>, kPipelineCount> pipelines_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreatePrimitive();
	void CreatePipeline();

};
