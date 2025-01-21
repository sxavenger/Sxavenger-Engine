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

//* c++
#include <memory>

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class AViewActor;

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

	void Draw(const DirectXThreadContext* context, const AViewActor* view);

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

	void SetVertexBuffer(const Vector4f& position, const Color4f& color);

};
