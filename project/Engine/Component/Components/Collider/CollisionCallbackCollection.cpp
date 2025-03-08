#include "CollisionCallbackCollection.h"

////////////////////////////////////////////////////////////////////////////////////////////
// CollisionCallbackCollection class methods
////////////////////////////////////////////////////////////////////////////////////////////

void CollisionCallbackCollection::SetOnCollisionFunctions(const std::string& x, const std::string& y, const OnCollisionCallbacks& functions) {
	callbacks_[x][y] = functions;
}

void CollisionCallbackCollection::CallbackOnCollisionEnter(ColliderComponent* rhs, ColliderComponent* lhs) const {
	if (!callbacks_.contains(rhs->GetTag()) || !callbacks_.at(rhs->GetTag()).contains(lhs->GetTag())) {
		return;
	}
	
	const auto& callback = callbacks_.at(rhs->GetTag()).at(lhs->GetTag()).enter;

	if (callback) {
		callback(rhs, lhs);
	};
}

void CollisionCallbackCollection::CallbackOnCollisionExit(ColliderComponent* rhs, ColliderComponent* lhs) const {
	if (!callbacks_.contains(rhs->GetTag()) || !callbacks_.at(rhs->GetTag()).contains(lhs->GetTag())) {
		return;
	}

	const auto& callback = callbacks_.at(rhs->GetTag()).at(lhs->GetTag()).exit;

	if (callback) {
		callback(rhs, lhs);
	};
}

void CollisionCallbackCollection::CallbackOnCollisionStay(ColliderComponent* rhs, ColliderComponent* lhs) const {
	if (!callbacks_.contains(rhs->GetTag()) || !callbacks_.at(rhs->GetTag()).contains(lhs->GetTag())) {
		return;
	}

	const auto& callback = callbacks_.at(rhs->GetTag()).at(lhs->GetTag()).stay;

	if (callback) {
		callback(rhs, lhs);
	};
}

std::pair<std::string, std::string> CollisionCallbackCollection::Sort(const std::string& x, const std::string& y) {
	return x < y ? std::make_pair(x, y) : std::make_pair(y, x);
}

std::pair<ColliderComponent*, ColliderComponent*> CollisionCallbackCollection::Sort(ColliderComponent* x, ColliderComponent* y) {
	return x->GetTag() < y->GetTag() ? std::make_pair(x, y) : std::make_pair(y, x);
}
