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
//! @brief componentのインスタンスを保管, 管理するクラス.
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

	//* factory *//
	using Factory = std::unordered_map<std::string, std::pair<const std::type_info*, std::function<std::unique_ptr<BaseComponent>(MonoBehaviour*)>>>;

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

	//* factory option *//

	template <Component _Ty>
	void RegisterFactory(); //!< todo: register componentと被るので名前変更

	ComponentIterator RegisterComponent(const std::string& component, MonoBehaviour* behavior);

	const std::type_info* GetComponentInfo(const std::string& component) const;

	const Factory& GetFactory() const { return factory_; }

	//* for each *//

	template <Component _Ty>
	void ForEach(const std::function<void(_Ty*)>& func);

	template <Component _Ty>
	void ForEachActive(const std::function<void(_Ty*)>& func);

	//* count *//

	template <Component _Ty>
	size_t GetComponentCount() const;
	//!< o(1)

	template <Component _Ty>
	size_t GetActiveComponentCount() const;
	//!< o(n)

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

	Factory factory_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// ComponentStorage class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <Component _Ty>
ComponentStorage::ComponentIterator ComponentStorage::RegisterComponent(MonoBehaviour* behavior) {
	RegisterFactory<_Ty>();
	constexpr const std::type_info* type = &typeid(_Ty);
	return storage_[type].emplace(storage_[type].end(), std::make_unique<_Ty>(behavior));
}

template <Component _Ty>
void ComponentStorage::UnregisterComponent(const ComponentIterator& iterator) {
	constexpr const std::type_info* type = &typeid(_Ty);
	storage_.at(type).erase(iterator);
}

template <Component _Ty>
void ComponentStorage::RegisterFactory() {
	constexpr const std::type_info* type = &typeid(_Ty);
	factory_.try_emplace(type->name(), type, [](MonoBehaviour* behaviour) { return std::make_unique<_Ty>(behaviour); });
}

template <Component _Ty>
void ComponentStorage::ForEach(const std::function<void(_Ty*)>& func) {
	constexpr const std::type_info* type = &typeid(_Ty);

	for (const auto& component : storage_[type]) {
		func(static_cast<_Ty*>(component.get()));
	}
}

template <Component _Ty>
void ComponentStorage::ForEachActive(const std::function<void(_Ty*)>& func) {
	constexpr const std::type_info* type = &typeid(_Ty);

	for (const auto& component : storage_[type]) {
		if (component->IsActive()) {
			func(static_cast<_Ty*>(component.get()));
		}
	}
}

template <Component _Ty>
inline size_t ComponentStorage::GetComponentCount() const {
	constexpr const std::type_info* type = &typeid(_Ty);

	if (!storage_.contains(type)) {
		return 0;
	}

	return storage_.at(type).size();
}

template<Component _Ty>
inline size_t ComponentStorage::GetActiveComponentCount() const {
	constexpr const std::type_info* type = &typeid(_Ty);

	if (!storage_.contains(type)) {
		return 0;
	}

	size_t count = 0;

	for (const auto& component : storage_.at(type)) {
		if (component->IsActive()) {
			count++;
		}
	}

	return count;
}

////////////////////////////////////////////////////////////////////////////////////////////
// singleton
////////////////////////////////////////////////////////////////////////////////////////////
static ComponentStorage* sComponentStorage = ComponentStorage::GetInstance();
