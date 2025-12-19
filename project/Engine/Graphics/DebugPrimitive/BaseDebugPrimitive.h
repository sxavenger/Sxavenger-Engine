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

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// BaseDebugPrimitive class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief デバッグ用プリミティブ基底クラス
class BaseDebugPrimitive {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// PrimitiveInput structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct PrimitiveInput {
		Vector4f position;
		Color4f  color;
		float thickness;
	};


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

SXAVENGER_ENGINE_NAMESPACE_END

