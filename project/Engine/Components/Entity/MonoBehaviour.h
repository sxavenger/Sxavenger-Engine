#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* entity
#include "BehaviourAddress.h"

//* component
#include "../Component/ComponentStorage.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/Utility/StreamLogger.h>
#include <Engine/System/Utility/RuntimeLogger.h>
#include <Engine/Editors/Editor/InspectorEditor.h>

//* lib
#include <Lib/Adapter/Json/JsonSerializer.h>

//* c++
#include <unordered_map>
#include <unordered_set>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// MonoBehaviour class
////////////////////////////////////////////////////////////////////////////////////////////
class MonoBehaviour final
	: public BaseInspector, public IJsonSerializer {
public:
	// TODO: BehaviourEntityに命名変更予定.

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////

	//* components
	using Components = std::unordered_map<const std::type_info*, ComponentStorage::ComponentIterator>;

	//* hierarchy
	using Hierarchy = std::unordered_set<BehaviourAddress, BehaviourAddress::Hash, BehaviourAddress::Hash>;
	// TODO: orderが必要ならlistに変更, iterator管理も必要

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	MonoBehaviour();
	~MonoBehaviour();

	//* behaviour option *//

	void SetActive(bool isActive);

	bool IsActive() const { return isActive_; }

	void SetRenamable(bool isRenamable);

	void SetName(const std::string& name);

	const std::string& GetName() const { return name_; }

	//* components option *//

	//! @brief componentを追加
	//! @tparam _Ty 追加するcomponentの型
	//! @return 追加したcomponentのptr
	template <Component _Ty>
	_Ty* AddComponent();

	//! @brief componentを追加
	//! @param[in] component 追加するcomponentの名前
	//! @return 追加したcomponentの基底ptr
	BaseComponent* AddComponent(const std::string& component);

	//! @brief componentを削除
	//! @tparam _Ty 削除するcomponentの型
	template <Component _Ty>
	void RemoveComponent();

	//! @brief componentを削除
	//! @param[in] type 削除するcomponentの型情報
	void RemoveComponent(const std::type_info* type);

	//! @brief componentを取得
	//! @tparam _Ty 取得するcomponentの型
	//! @retval componentが存在する: componentのptr
	//! @retval 存在しない場合: nullptr
	template <Component _Ty>
	const _Ty* GetComponent() const;

	//! @brief componentを取得
	//! @tparam _Ty 取得するcomponentの型
	//! @retval componentが存在する: componentのptr
	//! @retval 存在しない場合: nullptr
	template <Component _Ty>
	_Ty* GetComponent();

	//! @brief componentを取得
	//! @tparam _Ty 取得するcomponentの型
	//! @return componentのptr
	//! @throw componentが存在しない場合に例外をスローする
	template <Component _Ty>
	const _Ty* RequireComponent() const;

	//! @brief componentを取得
	//! @tparam _Ty 取得するcomponentの型
	//! @return componentのptr
	//! @throw componentが存在しない場合に例外をスローする
	template <Component _Ty>
	_Ty* RequireComponent();

	const Components& GetComponents() const { return components_; }
	Components& GetComponents() { return components_; }

	//* hierarchy [child] option *//

	//! @brief childを追加
	//! @param[in] child 追加するchild(所有権なし)
	void AddChild(const BehaviourAddress& child);

	//! @brief childを追加
	//! @param[in] child 追加するchild(所有権あり)
	void AddChild(BehaviourAddress&& child);

	//! @brief childの削除
	//! @param[in] child 削除するchild
	void RemoveChild(const BehaviourAddress& child);

	//! @brief childが存在するか
	//! @retval true 存在する
	//! @retval false 存在しない
	bool HasChild() const { return !children_.empty(); }

	//! @brief nameからchildを検索
	//! @param name 検索するchildのname
	//! @retval address 検索したchildのaddress
	//! @retval nullptr 存在しない
	BehaviourAddress FindChild(const std::string& name) const;

	//! @brief childrenを取得
	//! @return childrenのaddress集合
	const Hierarchy& GetChildren() const { return children_; }

	//* hierarchy [parent] option *//

	//! @brief parentが存在するか確認
	//! @retval true  存在する
	//! @retval false 存在しない
	bool HasParent() const { return parent_ != nullptr; }

	//! @brief parentを取得
	//! @retval address parentのaddress
	//! @retval nullptr parentが存在しない
	const BehaviourAddress& GetParent() const;

	//! @brief parentを取得
	//! @return parentのaddress
	//! @throw parentが存在しない場合に例外をスローする
	const BehaviourAddress& RequireParent() const;

	//* for each child *//

	void ForEachChild(const std::function<void(MonoBehaviour*)>& function) const;

	//* getter *//

	uintptr_t GetAddress() const { return reinterpret_cast<uintptr_t>(this); }
	//!< Warning: address受け渡しは内部でのみ使用する.

	//* inspector option *//

	void ShowInspector() override;

	virtual void LateUpdateInspector() override;

	void SetInspectable(const std::function<void()>& function) { inspectable_ = function; }
	void SetInspectable(std::nullptr_t) { inspectable_ = nullptr; }

	//* json serializer option *//

	json PerseToJson() const override;

	void InputJson(const json& data) override;

	void LoadComponent(const std::filesystem::path& filepath);

	void SaveComponent(const std::filesystem::path& filepath);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* name
	std::string name_ = "new behaviour";
	bool isRenamable_ = true; //!< systemとして使われない限りtrue.

	//* flag
	bool isActive_ = true;

	//* components
	Components components_;

	//* hierarchy
	BehaviourAddress parent_ = nullptr;
	Hierarchy children_;

	//* inspectable
	std::function<void()> inspectable_ = nullptr;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* hierarchy [pointer] helper *//

	void SetParent(MonoBehaviour* parent);

	void RemoveParent(const MonoBehaviour* parent);

	void RemoveChild(MonoBehaviour* child);

};

////////////////////////////////////////////////////////////////////////////////////////////
// MonoBehaviour class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <Component _Ty>
_Ty* MonoBehaviour::AddComponent() {
	// component typeの取得
	constexpr const std::type_info* type = &typeid(_Ty);

	// componentの追加
	if (!components_.contains(type)) {
		components_[type] = sComponentStorage->RegisterComponent<_Ty>(this);
		
	} else {
		RuntimeLogger::LogWarning("[MonoBehaviour]", "component is already added. component is only one.");
	}

	return static_cast<_Ty*>(components_[type]->get());
}

template <Component _Ty>
void MonoBehaviour::RemoveComponent() {
	// component typeの取得
	constexpr const std::type_info* type = &typeid(_Ty);


	if (!components_.contains(type)) { //!< componentが存在しない場合
		RuntimeLogger::LogWarning("[MonoBehaviour]", "component is not found. type: " + std::string(type->name()));
	}

	//!< componentの削除
	sComponentStorage->UnregisterComponent<_Ty>(components_[type]);
	components_.erase(type);
}

template <Component _Ty>
const _Ty* MonoBehaviour::GetComponent() const {
	//!< component typeの取得
	constexpr const std::type_info* type = &typeid(_Ty);

	if (!components_.contains(type)) { //!< componentが存在しない場合
		return nullptr;
	}

	//!< componentの取得
	return static_cast<_Ty*>(components_[type]->get());
}

template <Component _Ty>
_Ty* MonoBehaviour::GetComponent() {
	//!< component typeの取得
	constexpr const std::type_info* type = &typeid(_Ty);

	if (!components_.contains(type)) { //!< componentが存在しない場合
		return nullptr;
	}

	//!< componentの取得
	return static_cast<_Ty*>(components_[type]->get());
}

template <Component _Ty>
const _Ty* MonoBehaviour::RequireComponent() const {
	//!< component typeの取得
	constexpr const std::type_info* type = &typeid(_Ty);

	if (!components_.contains(type)) { //!< componentが存在しない場合
		StreamLogger::Exception("component is not found. type: " + std::string(type->name()));
	}

	//!< componentの取得
	return static_cast<_Ty*>(components_[type]->get());
}

template <Component _Ty>
_Ty* MonoBehaviour::RequireComponent() {
	//!< component typeの取得
	constexpr const std::type_info* type = &typeid(_Ty);

	if (!components_.contains(type)) { //!< componentが存在しない場合
		StreamLogger::Exception("component is not found. type: " + std::string(type->name()));
	}

	//!< componentの取得
	return static_cast<_Ty*>(components_[type]->get());
}

SXAVENGER_ENGINE_NAMESPACE_END
