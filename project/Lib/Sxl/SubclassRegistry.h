#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* sxl
#include "SxavengerLibrary.h"

//* c++
#include <unordered_map>
#include <typeinfo>
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Library namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_LIBRARY_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// SubclassRegistry class
////////////////////////////////////////////////////////////////////////////////////////////
template <typename _Base>
class SubclassRegistry {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	template <typename _Derived> requires std::derived_from<_Derived, _Base>
	void Insert();

	template <typename _Derived> requires std::derived_from<_Derived, _Base>
	bool Contains() const;

	template <typename _Derived> requires std::derived_from<_Derived, _Base>
	_Derived* Get() const;

	void Clear() { container_.clear(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unordered_map<const std::type_info*, std::unique_ptr<_Base>> container_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	template <typename _Derived> requires std::derived_from<_Derived, _Base>
	static _Derived* Cast(_Base* ptr) { return static_cast<_Derived*>(ptr); }

};

////////////////////////////////////////////////////////////////////////////////////////////
// SubclassRegistry class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template<typename _Base>
template<typename _Derived> requires std::derived_from<_Derived, _Base>
inline void SubclassRegistry<_Base>::Insert() {
	container_.emplace(&typeid(_Derived), std::make_unique<_Derived>());
}

template<typename _Base>
template<typename _Derived> requires std::derived_from<_Derived, _Base>
inline bool SubclassRegistry<_Base>::Contains() const {
	return container_.contains(&typeid(_Derived));
}

template<typename _Base>
template<typename _Derived> requires std::derived_from<_Derived, _Base>
inline _Derived* SubclassRegistry<_Base>::Get() const {
	if (!Contains<_Derived>()) {
		return nullptr;
	}

	return Cast<_Derived>(container_.at(&typeid(_Derived)).get());
}

SXAVENGER_LIBRARY_NAMESPACE_END


