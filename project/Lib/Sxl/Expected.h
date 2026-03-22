#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* sxl
#include "SxavengerLibrary.h"
#include "Exception.h"

//* lib
#include <Lib/CXXAttribute.h>

//* c++
#include <concepts>
#include <variant>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Library namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_LIBRARY_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// Unexpected class
////////////////////////////////////////////////////////////////////////////////////////////
template <class E>
class Unexpected {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Unexpected() = default;

	//* operator [assign] *//

	Unexpected(const E& error) : error_(error) {}
	Unexpected& operator=(const E& error) { error_ = error; }

	Unexpected(E&& error) : error_(std::move(error)) {}
	Unexpected& operator=(E&& error) { error_ = std::move(error); return *this; }

	template <class U> requires std::constructible_from<E, U>
	Unexpected(U&& error) : error_(std::forward<U>(error)) {}

	template <class U>requires std::assignable_from<E&, U>
	Unexpected& operator=(U&& error) { error_ = std::forward<U>(error); return *this; }

	//* option *//

	const E& Error() const { return error_; }

private:

	//=========================================================================================
	// public variables
	//=========================================================================================

	E error_;

};



////////////////////////////////////////////////////////////////////////////////////////////
// Expected class
////////////////////////////////////////////////////////////////////////////////////////////
template <class _Ty, class _Err>
class NODISCARD Expected {
	//!< c++23以上になる場合は, std::expectedを使用する.
	//!< [<expected>](https://cpprefjp.github.io/reference/expected.html)
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// State enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class State : bool {
		Success = 0,
		Error   = 1,
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Expected() = default;

	//* expected option *//

	State GetState() const;

	bool HasValue() const;

	const _Ty& Value() const;

	const _Err& Error() const;

	//* operator [assign] *//

	Expected(const _Ty& value) : value_(value) {}
	Expected& operator=(const _Ty& value) { value_ = value; return *this; }

	Expected(_Ty&& value) : value_(std::move(value)) {}
	Expected& operator=(_Ty&& value) { value_ = std::move(value); return *this; }

	Expected(const Unexpected<_Err>& error) : value_(error) {}
	Expected& operator=(const Unexpected<_Err>& error) { value_ = error; return *this; }

	Expected(Unexpected<_Err>&& error) : value_(std::move(error)) {}
	Expected& operator=(Unexpected<_Err>&& error) { value_ = std::move(error); return *this; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::variant<_Ty, Unexpected<_Err>> value_ = _Ty();

};

////////////////////////////////////////////////////////////////////////////////////////////
// Expected class methods
////////////////////////////////////////////////////////////////////////////////////////////

template<class _Ty, class _Err>
inline Expected<_Ty, _Err>::State Expected<_Ty, _Err>::GetState() const {
	return std::holds_alternative<_Ty>(value_) ? State::Success : State::Error;
}

template <class _Ty, class _Err>
bool Expected<_Ty, _Err>::HasValue() const {
	return Expected::GetState() == State::Success;
}

template <class _Ty, class _Err>
inline const _Ty& Expected<_Ty, _Err>::Value() const {
	if (GetState() == State::Error) {
		Exception::RuntimeError("expected value is invalid.");
	}

	return std::get<_Ty>(value_);
}

template<class _Ty, class _Err>
inline const _Err& Expected<_Ty, _Err>::Error() const {
	if (GetState() != State::Error) {
		Exception::RuntimeError("expected error value is invalid.");
	}

	return std::get<Unexpected<_Err>>(value_).Error();
}

SXAVENGER_LIBRARY_NAMESPACE_END
