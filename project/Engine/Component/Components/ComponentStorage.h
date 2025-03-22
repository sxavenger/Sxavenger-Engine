#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "BaseComponent.h"

//* c++
#include <typeinfo>
#include <unordered_map>
#include <memory>
#include <list>
#include <functional>

////////////////////////////////////////////////////////////////////////////////////////////
// ComponentStorage class
////////////////////////////////////////////////////////////////////////////////////////////
class ComponentStorage {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////

	//* component *//
	using ComponentContainer = std::list<std::unique_ptr<BaseComponent>>;
	using ComponentIterator  = ComponentContainer::iterator;

	//* storage *//
	using Storage = std::unordered_map<const std::type_info*, ComponentContainer>;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* storage option *//

	template <Component _Ty>
	ComponentIterator RegisterComponent(MonoBehaviour* behavior);

	template <Component _Ty>
	void UnregisterComponent(const ComponentIterator& iterator);

	void UnregisterComponent(const std::type_info* type, const ComponentIterator& iterator);

	//* for each *//

	template <Component _Ty>
	void ForEach(const std::function<void(_Ty*)>& func);

	//* getter *//

	template <Component _Ty>
	const ComponentContainer& GetComponentContainer() const { return storage_[&typeid(_Ty)]; }

	template <Component _Ty>
	ComponentContainer& GetComponentContainer() { return storage_[&typeid(_Ty)]; }

	//* singleton *//

	static ComponentStorage* GetInstance();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Storage storage_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// ComponentStorage class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <Component _Ty>
ComponentStorage::ComponentIterator ComponentStorage::RegisterComponent(MonoBehaviour* behavior) {
	constexpr std::type_info* type = &typeid(_Ty);

	if (!storage_.contains(type)) {
		storage_[type] = ComponentContainer();
	}

	return storage_[type].emplace(storage_[type].end(), std::make_unique<_Ty>(behavior));
}

template <Component _Ty>
void ComponentStorage::UnregisterComponent(const ComponentIterator& iterator) {
	constexpr std::type_info* type = &typeid(_Ty);

	storage_.at(type).erase(iterator);
}

template <Component _Ty>
void ComponentStorage::ForEach(const std::function<void(_Ty*)>& func) {
	constexpr std::type_info* type = &typeid(_Ty);

	for (const auto& component : storage_.at(type)) {
		func(static_cast<_Ty*>(component.get()));
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// singleton
////////////////////////////////////////////////////////////////////////////////////////////
static ComponentStorage* sComponentStorage = ComponentStorage::GetInstance();
