#include "CollisionManager.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../ComponentStorage.h"
#include "ColliderComponent.h"

////////////////////////////////////////////////////////////////////////////////////////////
// CollisionManager class methods
////////////////////////////////////////////////////////////////////////////////////////////

void CollisionManager::CheckCollision() {
	// colliderの状態を更新
	sComponentStorage->ForEach<ColliderComponent>([](ColliderComponent* collider) {
		collider->UpdateColliderState();
	});

	// componentsの取得
	auto& components = sComponentStorage->GetComponentContainer<ColliderComponent>();

	// collider同士の当たり判定
	for (auto itrA = components.begin(); itrA != components.end(); ++itrA) {
		for (auto itrB = std::next(itrA); itrB != components.end(); ++itrB) {

			// collider component の取得
			ColliderComponent* colliderA = static_cast<ColliderComponent*>(itrA->get());
			ColliderComponent* colliderB = static_cast<ColliderComponent*>(itrB->get());

			if (!(colliderA->IsActive() && colliderB->IsActive())) {
				continue; //!< どちらか一方が非アクティブの場合
			}

			// todo: hitgroupの適応
			
			const std::optional<CollisionBoundings::Boundings>& boundingA = colliderA->GetBoundings();
			const std::optional<CollisionBoundings::Boundings>& boundingB = colliderB->GetBoundings();

			if (!boundingA.has_value() || !boundingB.has_value()) {
				continue; //!< boundingが設定されていない場合
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
	sComponentStorage->ForEach<ColliderComponent>([](ColliderComponent* collider) {
		collider->CallbackOnCollision();
	});
}
