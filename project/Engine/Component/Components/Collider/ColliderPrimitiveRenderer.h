#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* collider
#include "CollisionDetection.h"

//* engine
#include <Engine/System/UI/ISystemDebugGui.h>
#include <Engine/System/DirectX/DxObject/DxGraphicsPipelineState.h>
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>
#include <Engine/System/DirectX/DxObject/DxVectorDimensionBuffer.h>
#include <Engine/System/DirectX/DirectXContext.h>
#include <Engine/Component/Components/Camera/CameraComponent.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ColliderPrimitiveSphere class
////////////////////////////////////////////////////////////////////////////////////////////
class ColliderPrimitiveSphere {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init();

	void Reset();

	void StackBuffer(const Vector3f& position, const CollisionBoundings::Sphere& sphere, const Color4f& color);

	void Render(const DirectXThreadContext* context, CameraComponent* component);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* pipeline *//

	std::unique_ptr<DxObject::ReflectionGraphicsPipelineState> pipeline_;

	//* buffer *//

	std::unique_ptr<DxObject::VertexDimensionBuffer<Vector4f>> vb_;
	std::unique_ptr<DxObject::VectorDimensionBuffer<std::pair<Matrix4x4, Color4f>>> buffer_;

	const uint32_t kSubdivision = 36;
	const float kRoundEvery     = (pi_v * 2.0f) / kSubdivision;

	uint32_t index_ = 0;
	const uint32_t kMaxBuffer = 256;

	//=========================================================================================
	// private variables
	//=========================================================================================

	void CreatePipeline();

};

////////////////////////////////////////////////////////////////////////////////////////////
// ColliderPrimitiveLine class
////////////////////////////////////////////////////////////////////////////////////////////
class ColliderPrimitiveLine {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init();

	void Reset();

	void StackLine(const Vector3f& x, const Vector3f& y, const Color4f& color);

	void Render(const DirectXThreadContext* context, CameraComponent* component);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================



};


////////////////////////////////////////////////////////////////////////////////////////////
// ColliderPrimitiveRenderer class
////////////////////////////////////////////////////////////////////////////////////////////
class ColliderPrimitiveRenderer {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init();

	void Term();

	void Render(const DirectXThreadContext* context, CameraComponent* component);

	void StackSphere(const Vector3f& position, const CollisionBoundings::Sphere& sphere, const Color4f& color);

	void SetImGuiCommand();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* primitives *//

	std::unique_ptr<ColliderPrimitiveSphere> sphere_;

	//* parameters *//

	Color4f enableColor_  = { 1.0f, 1.0f, 0.0f, 1.0f };
	Color4f disableColor_ = { 0.4f, 0.4f, 0.4f, 1.0f };

	bool isDisableRender_ = true;

};
