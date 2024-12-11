#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* collider
#include "Collider.h"

//* engine
#include <Engine/System/UI/ISystemDebugGui.h>

//* c++
#include <unordered_set>
#include "../../../Lib/Geometry/Vector4.h"

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

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* collider *//

	std::unordered_set<Collider*> colliders_;

	//* paraemter *//



	//=========================================================================================
	// private methods
	//=========================================================================================

	void SetupColliderState();

	void CheckAllCollision();
	void CheckCollisionPair(Collider* colliderA, Collider* colliderB);

	void CallBack();
};

////////////////////////////////////////////////////////////////////////////////////////////
// CollisionDrawer class
////////////////////////////////////////////////////////////////////////////////////////////
class CollisionDrawer {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void DrawCollider(const Collider* collider, const Color4f& color);

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