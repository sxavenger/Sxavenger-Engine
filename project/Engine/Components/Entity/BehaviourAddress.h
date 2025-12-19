#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Foundation.h>

//* c++
#include <cstdint>
#include <optional>
#include <functional>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class MonoBehaviour;

////////////////////////////////////////////////////////////////////////////////////////////
// BehaviourAddress class
////////////////////////////////////////////////////////////////////////////////////////////
class BehaviourAddress {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Hash structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Hash {
	public:

		//=========================================================================================
		// public operator
		//=========================================================================================

		size_t operator()(const BehaviourAddress& address) const noexcept;

		bool operator()(const BehaviourAddress& lhs, const BehaviourAddress& rhs) const noexcept;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Ownership enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Ownership : bool {
		Owned    = true,
		Borrowed = false
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constactor & destructor *//

	BehaviourAddress() noexcept = default;
	BehaviourAddress(std::nullptr_t) noexcept : address_(std::nullopt), ownership_(Ownership::Owned) {}
	BehaviourAddress(uintptr_t address, Ownership ownership = Ownership::Owned) noexcept : address_(address), ownership_(ownership) {}

	BehaviourAddress(const BehaviourAddress& other) noexcept;
	BehaviourAddress(BehaviourAddress&& other) noexcept;

	//* operator [compare] *//

	bool operator==(std::nullptr_t) const { return !address_.has_value(); }
	bool operator!=(std::nullptr_t) const { return address_.has_value(); }

	bool operator==(uintptr_t address) const { return address_ == std::make_optional(address); }
	bool operator!=(uintptr_t address) const { return address_ != std::make_optional(address); }

	bool operator==(Ownership ownership) const { return ownership_ == ownership; }
	bool operator!=(Ownership ownership) const { return ownership_ != ownership; }

	//* operator [assign] *//

	BehaviourAddress& operator=(const BehaviourAddress& other) noexcept;
	BehaviourAddress& operator=(BehaviourAddress&& other) noexcept;

	//* operator [access] *//

	MonoBehaviour* operator->() const;

	//* operator [cast] *//

	operator bool() const { return address_.has_value(); }

	//* getter *//

	MonoBehaviour* Get() const;
	//!< TODO: Reference型を用意する.

	Ownership GetOwnership() const { return ownership_; }
	
	uintptr_t GetAddress() const;
	//!< Warninig: 内部でのみ使用する.

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::optional<uintptr_t> address_ = std::nullopt;

	Ownership ownership_ = Ownership::Owned;

};

SXAVENGER_ENGINE_NAMESPACE_END
