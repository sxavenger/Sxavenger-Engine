#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* collider
#include "Collider.h"

//* engine
#include <Engine/System/UI/ISystemDebugGui.h>

//* lib
#include <Lib/Geometry/Color.h>

//* c++
#include <unordered_set>


////////////////////////////////////////////////////////////////////////////////////////////
// ColliderCollection class
////////////////////////////////////////////////////////////////////////////////////////////
class ColliderCollection
	: public ISystemDebugGui {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ColliderCollection()  = default;
	~ColliderCollection() = default;

	void CheckCollision();

	void Draw();

	void SystemDebugGui() override;

	//* collider option *//

	void SetCollider(Collider* collider);

	void EraseCollider(Collider* collider);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* collider *//

	std::unordered_set<Collider*> colliders_;

	//* paraemter *//

	Color4f activeColor_   = ToColor4<float>(0xFAFA00FF);
	Color4f inactiveColor_ = ToColor4<float>(0x808040FF);

	bool isDraw_ = true;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void SetupColliderState();

	void CheckAllCollision();
	void CheckCollisionPair(Collider* colliderA, Collider* colliderB);

	void CallBack();
};

////////////////////////////////////////////////////////////////////////////////////////////
// ColliderDrawer class
////////////////////////////////////////////////////////////////////////////////////////////
class ColliderDrawer {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static void DrawCollider(const Collider* collider, const Color4f& color);

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Visitor structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Visitor {

		//* member *//

		Vector3f position = {};
		Color4f color     = {};

		//* methods *//

		void operator()(const CollisionBoundings::Sphere& sphere) {
			DrawSphere(position, sphere, color);
		}

		void operator()(const CollisionBoundings::Capsule& capsule) {
			DrawCapsule(position, capsule, color);
		}

		void operator()(const CollisionBoundings::AABB& aabb) {
			DrawAABB(position, aabb, color);
		}

		void operator()(const CollisionBoundings::OBB& obb) {
			DrawOBB(position, obb, color);
		}
	};

	//=========================================================================================
	// private methods
	//=========================================================================================

	static void DrawSphere(const Vector3f& position, const CollisionBoundings::Sphere& sphere, const Color4f& color);

	static void DrawCapsule(const Vector3f& position, const CollisionBoundings::Capsule& capsule, const Color4f& color);

	static void DrawAABB(const Vector3f& position, const CollisionBoundings::AABB& aabb, const Color4f& color);

	static void DrawOBB(const Vector3f& position, const CollisionBoundings::OBB& obb, const Color4f& color);

};
