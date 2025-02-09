#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* sxl
#include "SxavengerLibrary.h"

//* c++
#include <optional>
#include <exception>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxl
////////////////////////////////////////////////////////////////////////////////////////////
_SXL_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// Optional class
////////////////////////////////////////////////////////////////////////////////////////////
template <class _Type>
class Optional {
	//* GPU側でも使えるoptional
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////

	using Bool = uint32_t;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	constexpr Optional() : has_(false) {} //!< default constructor
	constexpr Optional(const _Type& value) : has_(true), value_(value) {}       //!< copy constructor
	constexpr Optional(_Type&& value) : has_(true), value_(std::move(value)) {} //!< move constructor

	constexpr Optional(const Optional& other) : has_(other.has_), value_(other.value_) {}       //!< other optional copy constructor
	constexpr Optional(Optional&& other) : has_(other.has_), value_(std::move(other.value_)) {} //!< other optional copy constructor

	constexpr Optional(const std::optional<_Type>& other) : has_(other.has_value()), value_(other.value_or(NULL)) {} //!< std::optional copy constructor
	constexpr Optional(std::optional<_Type>&& other) : has_(other.has_value()), value_(std::move(other.value())) {}  //!< std::optional move constructor

	constexpr Optional(std::nullopt_t) : has_(false) {} //!< nullopt constructor

	constexpr ~Optional() {} //!< default destructor

	constexpr void Reset();

	constexpr bool HasValue() const { return has_; }

	constexpr const _Type& Value() const;
	constexpr _Type& Value();

	constexpr const _Type& ValueOr(const _Type& defaultValue) const;
	constexpr _Type& ValueOr(_Type& defaultValue);

	//=========================================================================================
	// public operators
	//=========================================================================================

	constexpr Optional& operator=(const _Type& value);
	constexpr Optional& operator=(_Type&& value);

	constexpr Optional& operator=(const Optional& other);
	constexpr Optional& operator=(Optional&& other);

	constexpr Optional& operator=(const std::optional<_Type>& other);
	constexpr Optional& operator=(std::optional<_Type>&& other);

	constexpr Optional& operator=(std::nullopt_t);

	constexpr operator bool() const { return HasValue(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Bool  has_   = false;
	_Type value_ = _Type{};

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CheckBadAccess() const;

};

////////////////////////////////////////////////////////////////////////////////////////////
// Optional class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template<class _Type>
inline constexpr void Optional<_Type>::Reset() {
	has_   = false;
	value_ = _Type{};
}

template<class _Type>
inline constexpr const _Type& Optional<_Type>::Value() const {
	CheckBadAccess();
	return value_;
}

template<class _Type>
inline constexpr _Type& Optional<_Type>::Value() {
	CheckBadAccess();
	return value_;
}

template<class _Type>
inline constexpr const _Type& Optional<_Type>::ValueOr(const _Type& defaultValue) const {
	return HasValue() ? Value() : defaultValue;
}

template<class _Type>
inline constexpr _Type& Optional<_Type>::ValueOr(_Type& defaultValue) {
	return HasValue() ? Value() : defaultValue;
}

template <class _Type>
inline constexpr Optional<_Type>& Optional<_Type>::operator=(const _Type& value) {
	has_   = true;
	value_ = value;
	return *this;
}

template <class _Type>
inline constexpr Optional<_Type>& Optional<_Type>::operator=(_Type&& value) {
	has_   = true;
	value_ = std::move(value);
	return *this;
}

template <class _Type>
inline constexpr Optional<_Type>& Optional<_Type>::operator=(const Optional& other) {
	has_   = other.has_;
	value_ = other.value_;
	return *this;
}

template <class _Type>
inline constexpr Optional<_Type>& Optional<_Type>::operator=(Optional&& other) {
	has_   = other.has_;
	value_ = std::move(other.value_);
	return *this;
}

template <class _Type>
inline constexpr Optional<_Type>& Optional<_Type>::operator=(const std::optional<_Type>& other) {
	has_   = other.has_value();
	value_ = other.value_or(_Type{});
	return *this;
}

template <class _Type>
inline constexpr Optional<_Type>& Optional<_Type>::operator=(std::optional<_Type>&& other) {
	has_   = other.has_value();
	value_ = std::move(other.value_or(_Type{}));
	return *this;
}

template <class _Type>
inline constexpr Optional<_Type>& Optional<_Type>::operator=(std::nullopt_t) {
	Reset();
	return *this;
}

template<class _Type>
inline void Optional<_Type>::CheckBadAccess() const {
	if (!HasValue()) {
		throw std::exception("Optional value is not set.");
	}
}

_SXL_NAMESPACE_END
