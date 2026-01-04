#include "BehaviourAddress.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* entity
#include "EntityBehaviourStorage.h"

//* engine
#include <Engine/System/Utility/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Hash structure
////////////////////////////////////////////////////////////////////////////////////////////

size_t BehaviourAddress::Hash::operator()(const BehaviourAddress& address) const noexcept {
	std::size_t hash = NULL;

	if (address != nullptr) {
		hash = std::hash<uintptr_t>()(address.GetAddress());
	}

	return hash;
}

bool BehaviourAddress::Hash::operator()(const BehaviourAddress& lhs, const BehaviourAddress& rhs) const noexcept {
	return lhs.address_ == rhs.address_;
}

////////////////////////////////////////////////////////////////////////////////////////////
// BehaviourAddress class methods
////////////////////////////////////////////////////////////////////////////////////////////

BehaviourAddress::BehaviourAddress(const BehaviourAddress& other) noexcept
	: address_(other.address_), ownership_(Ownership::Borrowed) {
}

BehaviourAddress::BehaviourAddress(BehaviourAddress&& other) noexcept
	: address_(other.address_), ownership_(other.ownership_) {
	other.address_   = std::nullopt;
	other.ownership_ = Ownership::Borrowed;
}

BehaviourAddress& BehaviourAddress::operator=(const BehaviourAddress& other) noexcept {
	address_   = other.address_;
	ownership_ = Ownership::Borrowed;

	return *this;
}

BehaviourAddress& BehaviourAddress::operator=(BehaviourAddress&& other) noexcept {
	address_   = other.address_;
	ownership_ = other.ownership_;

	other.address_   = std::nullopt;
	other.ownership_ = Ownership::Owned;

	return *this;
}

EntityBehaviour* BehaviourAddress::operator->() const {
	StreamLogger::AssertA(address_.has_value(), "behaviour address is nullptr.");
	return sEntityBehaviourStorage->GetBehaviour(address_.value());
}

EntityBehaviour* BehaviourAddress::Get() const {
	if (!address_.has_value()) {
		return nullptr;
	}

	return sEntityBehaviourStorage->GetBehaviour(address_.value());
}

uintptr_t BehaviourAddress::GetAddress() const {
	StreamLogger::AssertA(address_.has_value(), "behaviour address is nullptr.");
	return address_.value();
}
