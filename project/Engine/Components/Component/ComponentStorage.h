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
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// ComponentStorage class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief componentの格納/管理クラス
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
	using Factory = std::unordered_map<std::string, std::pair<const std::type_info*, std::function<std::unique_ptr<BaseComponent>(EntityBehaviour*)>>>;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* storage option *//

	//! @brief componentを登録
	//! @tparam _Ty componentの型
	//! @param[in] behavior componentを所有するEntity
	//! @return 登録したcomponentのiterator
	template <Component _Ty>
	ComponentIterator RegisterComponent(EntityBehaviour* behavior);

	//! @brief componentを登録解除
	//! @tparam _Ty componentの型
	//! @param[in] iterator 登録解除するcomponentのiterator
	template <Component _Ty>
	void UnregisterComponent(const ComponentIterator& iterator);

	//! @brief componentを登録解除
	//! @param[in] type 登録解除するcomponentの型情報
	//! @param[in] iterator 登録解除するcomponentのiterator
	void UnregisterComponent(const std::type_info* type, const ComponentIterator& iterator);

	//* factory option *//

	//! @brief componentのfactoryを登録
	template <Component _Ty>
	void RegisterFactory();

	//! @brief componentを登録
	//! @param[in] component componentの名前
	//! @param[in] behavior componentを所有するEntity
	//! @return 登録したcomponentのiterator
	ComponentIterator RegisterComponent(const std::string& component, EntityBehaviour* behavior);

	const std::type_info* GetComponentInfo(const std::string& component) const;

	const Factory& GetFactory() const { return factory_; }

	//* for each *//

	//! @brief 全てのcomponentに対してfuncを実行する
	//! @tparam _Ty componentの型
	//! @param func 実行関数
	template <Component _Ty>
	void ForEach(const std::function<void(_Ty*)>& func);

	//! @brief Activeなcomponentに対してfuncを実行する
	//! @tparam _Ty componentの型
	//! @param func 実行関数
	template <Component _Ty>
	void ForEachActive(const std::function<void(_Ty*)>& func);

	//* count *//

	//! @brief componentの数を取得
	//! @tparam _Ty componentの型
	//! @return componentの数
	//! @note: 計算量 o(1)
	template <Component _Ty>
	size_t GetComponentCount() const;

	//! @brief Activeなcomponentの数を取得
	//! @tparam _Ty componentの型
	//! @return Activeなcomponentの数
	//! @note: 計算量 o(n)
	template <Component _Ty>
	size_t GetActiveComponentCount() const;

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
ComponentStorage::ComponentIterator ComponentStorage::RegisterComponent(EntityBehaviour* behavior) {
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
	factory_.try_emplace(type->name(), type, [](EntityBehaviour* behaviour) { return std::make_unique<_Ty>(behaviour); });
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

SXAVENGER_ENGINE_NAMESPACE_END
