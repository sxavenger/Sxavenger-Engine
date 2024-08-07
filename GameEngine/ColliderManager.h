#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// collider base
#include <Collider.h>

// collisionDetection
#include <CollisionDetection.h>

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

	ColliderManager()  = default; //!< gameすべての当たり判定管理する場合, シングルトンでいいかも
	~ColliderManager() { Term(); }

	void Init();

	void Term();

	void Update();

	//* collders container *//

	void SetCollider(Collider* collider) { colliders_.emplace(collider); }

	void EraseCollider(Collider* collider) { colliders_.erase(collider); }

	static ColliderManager* GetInstance(); // test

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

};