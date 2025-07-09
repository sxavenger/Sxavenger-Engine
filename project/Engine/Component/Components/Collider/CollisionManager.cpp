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

	// componentsの取得
	auto& components = sComponentStorage->GetComponentContainer<ColliderComponent>();

	// collider同士の当たり判定
	std::vector<ComponentStorage::ComponentContainer::iterator> itrs;
	for (auto itr = components.begin(); itr != components.end(); ++itr) {
		itrs.emplace_back(itr);
	}

	std::for_each(std::execution::par, itrs.begin(), itrs.end(), [&](const auto& itrA) {
		size_t index = &itrA - std::addressof(itrs.front());

		// collider component A の取得
		ColliderComponent* colliderA = static_cast<ColliderComponent*>((*itrs[index]).get());

		if (!colliderA->IsActive()) {
			return; //!< 非アクティブの場合
		}

		// boundingを取得
		const std::optional<CollisionBoundings::Boundings>& boundingA = colliderA->GetBoundings();

		if (!boundingA.has_value()) {
			return; //!< boundingが設定されていない場合
		}

		for (size_t i = index + 1; i < itrs.size(); ++i) {

			// collider component B の取得
			ColliderComponent* colliderB = static_cast<ColliderComponent*>((*itrs[i]).get());

			if (!colliderB->IsActive()) {
				continue; //!< 非アクティブの場合
			}

			// boundingを取得
			const std::optional<CollisionBoundings::Boundings>& boundingB = colliderB->GetBoundings();

			if (!boundingB.has_value()) {
				continue; //!< boundingが設定されていない場合
			}

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
	});

	// colliderのcallback
	sComponentStorage->ForEach<ColliderComponent>([this](ColliderComponent* collider) {
		collider->CallbackOnCollision(this);
	});
}

CollisionManager* CollisionManager::GetInstance() {
	static CollisionManager instance;
	return &instance;
}
