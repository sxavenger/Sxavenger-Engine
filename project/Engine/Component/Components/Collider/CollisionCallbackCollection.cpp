#include "CollisionCallbackCollection.h"

////////////////////////////////////////////////////////////////////////////////////////////
// CollisionCallbackCollection class methods
////////////////////////////////////////////////////////////////////////////////////////////

void CollisionCallbackCollection::SetOnCollisionFunctions(const std::string& tagA, const std::string& tagB, const OnCollisionCallbacks& functions) {
	callbacks_[tagA][tagB] = functions;
}

void CollisionCallbackCollection::CallbackOnCollisionEnter(ColliderComponent* lhs, ColliderComponent* rhs) const {
	if (!callbacks_.contains(lhs->GetTag()) || !callbacks_.at(lhs->GetTag()).contains(rhs->GetTag())) {
		return;
	}
	
	const auto& callback = callbacks_.at(lhs->GetTag()).at(rhs->GetTag()).enter;

	if (callback) {
		callback(lhs, rhs);
	};
}

void CollisionCallbackCollection::CallbackOnCollisionExit(ColliderComponent* lhs, ColliderComponent* rhs) const {
	if (!callbacks_.contains(lhs->GetTag()) || !callbacks_.at(lhs->GetTag()).contains(rhs->GetTag())) {
		return;
	}

	const auto& callback = callbacks_.at(lhs->GetTag()).at(rhs->GetTag()).exit;

	if (callback) {
		callback(lhs, rhs);
	};
}

void CollisionCallbackCollection::CallbackOnCollisionStay(ColliderComponent* lhs, ColliderComponent* rhs) const {
	if (!callbacks_.contains(lhs->GetTag()) || !callbacks_.at(lhs->GetTag()).contains(rhs->GetTag())) {
		return;
	}

	const auto& callback = callbacks_.at(lhs->GetTag()).at(rhs->GetTag()).stay;

	if (callback) {
		callback(lhs, rhs);
	};
}

bool CollisionCallbackCollection::CheckRegistered(ColliderComponent* lhs, ColliderComponent* rhs) const {
	if (callbacks_.contains(lhs->GetTag()) && callbacks_.at(lhs->GetTag()).contains(rhs->GetTag())) {
		return true;
	}

	if (callbacks_.contains(rhs->GetTag()) && callbacks_.at(rhs->GetTag()).contains(lhs->GetTag())) {
		return true;
	}

	return false;
}

std::pair<std::string, std::string> CollisionCallbackCollection::Sort(const std::string& x, const std::string& y) {
	return x < y ? std::make_pair(x, y) : std::make_pair(y, x);
}

std::pair<ColliderComponent*, ColliderComponent*> CollisionCallbackCollection::Sort(ColliderComponent* x, ColliderComponent* y) {
	return x->GetTag() < y->GetTag() ? std::make_pair(x, y) : std::make_pair(y, x);
}
