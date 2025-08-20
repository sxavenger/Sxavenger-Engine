#include "CollisionManager.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../ComponentStorage.h"
#include "ColliderComponent.h"

//* c++
#include <execution>

////////////////////////////////////////////////////////////////////////////////////////////
// CollisionManager class methods
////////////////////////////////////////////////////////////////////////////////////////////

void CollisionManager::CheckCollision() {

	// colliderの状態を更新
	sComponentStorage->ForEach<ColliderComponent>([](ColliderComponent* collider) {
		collider->UpdateColliderState();
	});

	// containerの取得
	const auto& container = sComponentStorage->GetComponentContainer<ColliderComponent>();

	// collider同士の当たり判定
	for (auto itrA = container.begin(); itrA != container.end(); ++itrA) {
		ColliderComponent* colliderA = static_cast<ColliderComponent*>(itrA->get());

		if (!colliderA->IsActive() || !colliderA->IsEnable()) {
			continue; //!< 無効なColliderはスキップ
		}

		// boundingを取得
		const std::optional<CollisionBoundings::Boundings>& boundingA = colliderA->GetBoundings();

		if (!boundingA.has_value()) {
			return; //!< boundingが設定されていない場合
		}

		for (auto itrB = std::next(itrA); itrB != container.end(); ++itrB) {
			ColliderComponent* colliderB = static_cast<ColliderComponent*>(itrB->get());

			if (!colliderB->IsActive() || !colliderB->IsEnable()) {
				continue; //!< 無効なColliderはスキップ
			}

			// boundingを取得
			const std::optional<CollisionBoundings::Boundings>& boundingB = colliderB->GetBoundings();

			if (!boundingB.has_value()) {
				return; //!< boundingが設定されていない場合
			}

			//* 当たり判定 *//

			if (!CollisionCallbackCollection::CheckRegistered(colliderA, colliderB)) {
				continue; //!< 登録されてない場合
			}

			// bounding同士の当たり判定
			bool isCollision = CollisionDetection::CheckCollision(
				colliderA->GetTransform()->GetPosition(), boundingA.value(),
				colliderB->GetTransform()->GetPosition(), boundingB.value()
			);

			if (!isCollision) {
				continue; //!< collider同士が当たっていない場合
			}

			colliderA->OnCollision(colliderB);
			colliderB->OnCollision(colliderA);
		}
	}

	// colliderのcallback
	sComponentStorage->ForEachActive<ColliderComponent>([this](ColliderComponent* collider) {
		collider->CallbackOnCollision(this);
	});
}

CollisionManager* CollisionManager::GetInstance() {
	static CollisionManager instance;
	return &instance;
}
