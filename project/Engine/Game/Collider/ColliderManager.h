#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// collider base
#include "Collider.h"

// collisionDetection
#include "CollisionDetection.h"

// c++
#include <unordered_set>

////////////////////////////////////////////////////////////////////////////////////////////
// ColliderManager class
////////////////////////////////////////////////////////////////////////////////////////////
class ColliderManager final {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ColliderManager()  = default;
	~ColliderManager() = default;

	void Init();

	void Term();

	void Update();

	void DrawColliders(const Color4f& color = ToColor4f(0xFAFA00FF));

	//* collders container *//

	void SetCollider(Collider* collider) { colliders_.emplace(collider); }

	void EraseCollider(Collider* collider) { colliders_.erase(collider); }

	void ClearColliders() { colliders_.clear(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unordered_set<Collider*> colliders_;
	// todo: layerを追加して当たり判定にfilterをかける

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CheckAllCollision();

	void CheckCollisionPair(Collider* colliderA, Collider* colliderB);

	void DrawCollider(const Collider* const collider, const Color4f& color) const;
	// todo: primitiveに移動
	void DrawSphereCollider(const Vector3f& position, const CollisionBoundings::Sphere& sphere, const Color4f& color) const;
	void DrawAABBCollider(const Vector3f& position, const CollisionBoundings::AABB& aabb, const Color4f& color) const;

};