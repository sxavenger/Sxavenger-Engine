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

//* lib
#include <Lib/Geometry/Color.h>

////////////////////////////////////////////////////////////////////////////////////////////
// forward
////////////////////////////////////////////////////////////////////////////////////////////
class ACameraActor;

////////////////////////////////////////////////////////////////////////////////////////////
// ColliderPrimitiveDrawer class
////////////////////////////////////////////////////////////////////////////////////////////
class ColliderPrimitiveDrawer {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ColliderPrimitiveDrawer() = default;
	~ColliderPrimitiveDrawer() = default;

	void Init();

	void DrawCollider(const std::optional<CollisionBoundings::Boundings>& boundings, const Vector3f& position, const Color4f& color);

	void DrawSphere(const Vector3f& position, const CollisionBoundings::Sphere& sphere, const Color4f& color);

	void DrawCapsule(const Vector3f& position, const CollisionBoundings::Capsule& capsule, const Color4f& color);

	void DrawAABB(const Vector3f& position, const CollisionBoundings::AABB& aabb, const Color4f& color);

	void DrawOBB(const Vector3f& position, const CollisionBoundings::OBB& obb, const Color4f& color);

	//* draw option *//

	void Render(const DirectXThreadContext* context, ACameraActor* actor);

	void Clear();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<DxObject::VertexDimensionBuffer<Vector4f>> sphereVB_;
	std::unique_ptr<DxObject::VectorDimensionBuffer<std::pair<Matrix4x4, Color4f>>> sphereBuffer_;

	//* pipeline *//

	std::unique_ptr<DxObject::ReflectionGraphicsPipelineState> pipeline_;

	//* parameter *//

	const uint32_t kSphereSubdivision = 24;
	const float kSphereRoundEvery = pi_v * 2.0f / kSphereSubdivision;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateSphereIA();

	void CreatePipeline();

};


////////////////////////////////////////////////////////////////////////////////////////////
// ColliderPrimitiveDrawerVisitor class
////////////////////////////////////////////////////////////////////////////////////////////
class ColliderPrimitiveDrawerVisitor {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ColliderPrimitiveDrawerVisitor()  = default;
	~ColliderPrimitiveDrawerVisitor() = default;

	void operator()(const CollisionBoundings::Sphere& sphere);
	void operator()(const CollisionBoundings::Capsule& capsule);
	void operator()(const CollisionBoundings::AABB& aabb);
	void operator()(const CollisionBoundings::OBB& obb);

	//=========================================================================================
	// public variables
	//=========================================================================================

	Vector3f position = {};
	Color4f color     = {};

	ColliderPrimitiveDrawer* drawer = nullptr;

private:
};
