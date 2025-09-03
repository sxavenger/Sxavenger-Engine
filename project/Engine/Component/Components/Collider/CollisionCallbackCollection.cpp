#include "CollisionCallbackCollection.h"

////////////////////////////////////////////////////////////////////////////////////////////
// CollisionCallbackCollection class methods
////////////////////////////////////////////////////////////////////////////////////////////

void CollisionCallbackCollection::SetOnCollisionFunctions(const std::string& tagA, const std::string& tagB, const OnCollisionCallbacks& functions) {
	callbacks_[tagA][tagB] = functions;
}

void CollisionCallbackCollection::SetOnCollisionFunctionEnter(const std::string& tagA, const std::string& tagB, const OnCollisionFunction& function) {
	callbacks_[tagA][tagB].enter = function;
}

void CollisionCallbackCollection::SetOnCollisionFunctionExit(const std::string& tagA, const std::string& tagB, const OnCollisionFunction& function) {
	callbacks_[tagA][tagB].exit = function;
}

void CollisionCallbackCollection::SetOnCollisionFunctionStay(const std::string& tagA, const std::string& tagB, const OnCollisionFunction& function) {
	callbacks_[tagA][tagB].stay = function;
}

void CollisionCallbackCollection::CallbackOnCollisionEnter(ColliderComponent* lhs, ColliderComponent* rhs) const {
	if (!Contains(lhs->GetTag(), rhs->GetTag())) {
		return;
	}
	
	const auto& callback = callbacks_.at(lhs->GetTag()).at(rhs->GetTag()).enter;

	if (callback) {
		callback(lhs, rhs);
	};
}

void CollisionCallbackCollection::CallbackOnCollisionExit(ColliderComponent* lhs, ColliderComponent* rhs) const {
	if (!Contains(lhs->GetTag(), rhs->GetTag())) {
		return;
	}

	const auto& callback = callbacks_.at(lhs->GetTag()).at(rhs->GetTag()).exit;

	if (callback) {
		callback(lhs, rhs);
	};
}

void CollisionCallbackCollection::CallbackOnCollisionStay(ColliderComponent* lhs, ColliderComponent* rhs) const {
	if (!Contains(lhs->GetTag(), rhs->GetTag())) {
		return;
	}

	const auto& callback = callbacks_.at(lhs->GetTag()).at(rhs->GetTag()).stay;

	if (callback) {
		callback(lhs, rhs);
	};
}

bool CollisionCallbackCollection::CheckRegistered(ColliderComponent* lhs, ColliderComponent* rhs) const {
	return Contains(lhs->GetTag(), rhs->GetTag());
}

bool CollisionCallbackCollection::Contains(const std::string& tagA, const std::string& tagB) const {
	return callbacks_.contains(tagA) && callbacks_.at(tagA).contains(tagB);
}

std::pair<std::string, std::string> CollisionCallbackCollection::Sort(const std::string& x, const std::string& y) {
	return x < y ? std::make_pair(x, y) : std::make_pair(y, x);
}

std::pair<ColliderComponent*, ColliderComponent*> CollisionCallbackCollection::Sort(ColliderComponent* x, ColliderComponent* y) {
	return x->GetTag() < y->GetTag() ? std::make_pair(x, y) : std::make_pair(y, x);
}
