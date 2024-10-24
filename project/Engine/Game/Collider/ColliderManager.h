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

	void DrawColliders();

	//* collders container *//

	void SetCollider(Collider* collider) { colliders_.emplace(collider); }

	void EraseCollider(Collider* collider) { colliders_.erase(collider); }

	void ClearColliders() { colliders_.clear(); }

	//* getter *//

	const std::unordered_set<Collider*>& GetColliders() const { return colliders_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unordered_set<Collider*> colliders_;
	// todo: layerを追加して当たり判定にfilterをかける

	Color4f color_ = { 1.0f, 1.0f, 0.0f, 1.0f };

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CheckAllCollision();

	void CheckCollisionPair(Collider* colliderA, Collider* colliderB);

	void DrawCollider(const Collider* const collider) const;
	// todo: primitiveに移動
	void DrawSphereCollider(const Vector3f& position, const CollisionBoundings::Sphere& sphere) const;
	void DrawAABBCollider(const Vector3f& position, const CollisionBoundings::AABB& aabb) const;

};