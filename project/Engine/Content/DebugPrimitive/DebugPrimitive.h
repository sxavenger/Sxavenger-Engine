#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>
#include <Engine/System/DirectX/DxObject/DxGraphicsPipelineState.h>
#include <Engine/System/DirectX/DirectXContext.h>

//* lib
#include <Lib/Geometry/Vector4.h>
#include <Lib/Geometry/Vector3.h>
#include <Lib/Geometry/Color4.h>

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

	void Draw(const DirectXThreadContext* context, const CameraComponent* camera);

	void Reset();

	void CountBufferOffset();

protected:

	////////////////////////////////////////////////////////////////////////////////////////////
	// PrimitiveInput structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct PrimitiveInput {
		Vector4f position;
		Color4f  color;
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

	void SetVertexBuffer(const Vector3f& position, const Color4f& color);

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

	void PushLine(const Vector3f& v1, const Vector3f& v2, const Color4f& color);

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

	void DrawToScene(const DirectXThreadContext* context, const CameraComponent* camera);

	void ResetPrimitive();

	void PushLine(const Vector3f& v1, const Vector3f& v2, const Color4f& color);

	//* drawer options *//

	void PushGrid(const Vector3f& center, float size);

	void PushAxis(const Vector3f& center, float length);

	void PushBox(const Vector3f& min, const Vector3f& max, const Color4f& color);

	void PushSphere(const Vector3f& center, float radius, const Color4f& color);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* primitive *//

	std::unique_ptr<DebugPrimitiveLine> line_;

	//* pipeline *//

	std::unique_ptr<DxObject::GraphicsPipelineState> pipeline_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreatePrimitive();
	void CreatePipeline();

};
