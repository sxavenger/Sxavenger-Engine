#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* sxl
#include "SxavengerLibrary.h"

//* c++
#include <variant>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxl
////////////////////////////////////////////////////////////////////////////////////////////
_SXL_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// Holder class
////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
class Holder {
	//* ptrまたはvalueを保持するクラス
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////
	using Type = std::variant<T, T*>;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Holder()  = default;
	~Holder() = default;

	Holder(const T& value) : value_(value) {}
	Holder(T* ptr) : value_(ptr) {}

	//* operator *//

	T& operator*();
	const T& operator*() const;

	//* gettter *//

	T& Value();
	const T& Value() const;

	T* Ptr();
	const T* Ptr() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Type value_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// Holder class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
inline T& Holder<T>::operator*() {
	if (std::holds_alternative<T>(value_)) {
		return std::get<T>(value_);
	}

	return *std::get<T*>(value_);
}

template <class T>
inline const T& Holder<T>::operator*() const {
	if (std::holds_alternative<T>(value_)) {
		return std::get<T>(value_);
	}

	return *std::get<T*>(value_);
}

template <class T>
inline T& Holder<T>::Value() {
	if (std::holds_alternative<T>(value_)) {
		return std::get<T>(value_);
	}

	return *std::get<T*>(value_);
}

template <class T>
inline const T& Holder<T>::Value() const {
	if (std::holds_alternative<T>(value_)) {
		return std::get<T>(value_);
	}

	return *std::get<T*>(value_);
}

template <class T>
inline T* Holder<T>::Ptr() {
	if (std::holds_alternative<T>(value_)) {
		return &std::get<T>(value_);
	}

	return std::get<T*>(value_);
}

template<class T>
inline const T* Holder<T>::Ptr() const {
	if (std::holds_alternative<T>(value_)) {
		return &std::get<T>(value_);
	}

	return std::get<T*>(value_);
}

_SXL_NAMESPACE_END
