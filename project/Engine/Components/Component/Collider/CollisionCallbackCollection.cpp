#include "CollisionCallbackCollection.h"
SXAVENGER_ENGINE_USING

////////////////////////////////////////////////////////////////////////////////////////////
// CollisionCallbackCollection class methods
////////////////////////////////////////////////////////////////////////////////////////////

void CollisionCallbackCollection::SetOnCollisionFunctions(const std::string& tagA, const std::string& tagB, const OnCollisionCallbacks& functions) {
	callbacks_[tagA][tagB] = functions;
}

void CollisionCallbackCollection::SetOnCollisionFunctionEnter(const std::string& tagA, const std::string& tagB, const OnCollisionFunctionPenetration& function) {
	callbacks_[tagA][tagB].enter = function;
}

void CollisionCallbackCollection::SetOnCollisionFunctionEnter(const std::string& tagA, const std::string& tagB, const OnCollisionFunction& function) {
	callbacks_[tagA][tagB].enter = std::bind(function, std::placeholders::_1, std::placeholders::_2);
}

void CollisionCallbackCollection::SetOnCollisionFunctionExit(const std::string& tagA, const std::string& tagB, const OnCollisionFunctionPenetration& function) {
	callbacks_[tagA][tagB].exit = function;
}

void CollisionCallbackCollection::SetOnCollisionFunctionExit(const std::string& tagA, const std::string& tagB, const OnCollisionFunction& function) {
	callbacks_[tagA][tagB].exit = std::bind(function, std::placeholders::_1, std::placeholders::_2);
}

void CollisionCallbackCollection::SetOnCollisionFunctionStay(const std::string& tagA, const std::string& tagB, const OnCollisionFunctionPenetration& function) {
	callbacks_[tagA][tagB].stay = function;
}

void CollisionCallbackCollection::SetOnCollisionFunctionStay(const std::string& tagA, const std::string& tagB, const OnCollisionFunction& function) {
	callbacks_[tagA][tagB].stay = std::bind(function, std::placeholders::_1, std::placeholders::_2);
}

void CollisionCallbackCollection::CallbackOnCollisionEnter(ColliderComponent* lhs, ColliderComponent* rhs, const CollisionDetection::Penetration& penetration) const {
	if (!Contains(lhs->GetTag(), rhs->GetTag())) {
		return;
	}

	const auto& callback = callbacks_.at(lhs->GetTag()).at(rhs->GetTag()).enter;

	if (callback) {
		callback(lhs, rhs, penetration);
	};
}

void CollisionCallbackCollection::CallbackOnCollisionExit(ColliderComponent* lhs, ColliderComponent* rhs, const CollisionDetection::Penetration& penetration) const {
	if (!Contains(lhs->GetTag(), rhs->GetTag())) {
		return;
	}

	const auto& callback = callbacks_.at(lhs->GetTag()).at(rhs->GetTag()).exit;

	if (callback) {
		callback(lhs, rhs, penetration);
	};
}

void CollisionCallbackCollection::CallbackOnCollisionStay(ColliderComponent* lhs, ColliderComponent* rhs, const CollisionDetection::Penetration& penetration) const {
	if (!Contains(lhs->GetTag(), rhs->GetTag())) {
		return;
	}

	const auto& callback = callbacks_.at(lhs->GetTag()).at(rhs->GetTag()).stay;

	if (callback) {
		callback(lhs, rhs, penetration);
	};

}

bool CollisionCallbackCollection::CheckRegistered(const ColliderComponent* lhs, const ColliderComponent* rhs) const {
	return Contains(lhs->GetTag(), rhs->GetTag()) || Contains(rhs->GetTag(), lhs->GetTag());
}

bool CollisionCallbackCollection::Contains(const std::string& tagA, const std::string& tagB) const {
	return callbacks_.contains(tagA) && callbacks_.at(tagA).contains(tagB);
}
