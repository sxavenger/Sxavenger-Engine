#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "BaseComponent.h"

//* c++
#include <typeindex>
#include <unordered_map>
#include <memory>
#include <list>

////////////////////////////////////////////////////////////////////////////////////////////
// ComponentStorage class
////////////////////////////////////////////////////////////////////////////////////////////
class ComponentStorage {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////
	using ComponentContainer = std::list<std::unique_ptr<BaseComponent>>;
	using ComponentIterator  = ComponentContainer::iterator;


public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* storage option *//

	template <Component T>
	ComponentIterator RegisterComponent(MonoBehaviour* behavior);

	template <Component T>
	void UnregisterComponent(const ComponentIterator& iterator);

	void UnregisterComponent(std::type_index type, const ComponentIterator& iterator);

	//* getter *//

	template <Component T>
	ComponentContainer& GetComponentContainer() { return storage_[typeid(T)]; }

	//* singleton *//

	static ComponentStorage* GetInstance();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unordered_map<std::type_index, ComponentContainer> storage_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// ComponentStorage class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <Component T>
ComponentStorage::ComponentIterator ComponentStorage::RegisterComponent(MonoBehaviour* behavior) {
	std::type_index type = typeid(T);

	if (!storage_.contains(type)) {
		storage_[type] = ComponentContainer();
	}

	auto component = std::make_unique<T>(behavior); //!< todo: std::make_unique<T>(args...);
	return storage_[type].emplace(storage_[type].end(), std::move(component));
}

template <Component T>
inline void ComponentStorage::UnregisterComponent(const ComponentIterator& iterator) {
	storage_[typeid(T)].erase(iterator);
}

////////////////////////////////////////////////////////////////////////////////////////////
// static variables
////////////////////////////////////////////////////////////////////////////////////////////
static ComponentStorage* sComponentStorage = ComponentStorage::GetInstance();
