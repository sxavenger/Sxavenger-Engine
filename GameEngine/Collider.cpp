#include "Collider.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// managers
#include <ColliderManager.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Collider base class methods
////////////////////////////////////////////////////////////////////////////////////////////

Collider::Collider() {
	ColliderManager::GetInstance()->SetCollider(this);
}

Collider::~Collider() {
	ColliderManager::GetInstance()->EraseCollider(this);
}

void Collider::TEST_Init() {
	bounding_ = CollisionBoundings::Sphere{
		.radius = 1.0f
	};
}

void Collider::CallOnCollisionMethods() {
	for (auto it = states_.begin(); it != states_.end();) {

		if (it->second.isHit && !it->second.isPreHit) { //!< colliderが当たったかどうか
			OnCollisionEnter(it->first);

		} else if (!it->second.isHit && it->second.isPreHit) { //!< colliderが離れたかどうか
			OnCollisionExit(it->first);
		}

		//!< 次フレームの準備
		it->second.isPreHit = it->second.isHit;
		it->second.isHit    = false;

		// 当たらなくなったので削除
		if (!it->second.isHit && !it->second.isPreHit) {
			it = states_.erase(it);
			continue;
		}

		++it;
	}
}

void Collider::OnCollision(Collider* other) {
	states_[other].isHit = true; //!< 現在frameで当たった
}
