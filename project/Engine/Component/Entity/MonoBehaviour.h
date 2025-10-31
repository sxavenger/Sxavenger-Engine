#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../Components/ComponentStorage.h"

//* engine
#include <Engine/System/Utility/Logger.h>
#include <Engine/Editor/Editors/InspectorEditor.h>

//* lib
#include <Lib/Sxl/OptimizedMap.h>

//* c++
#include <typeinfo>
#include <unordered_map>
#include <string>
#include <list>
#include <variant>
#include <memory>
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////
// MonoBehaviour class
////////////////////////////////////////////////////////////////////////////////////////////
class MonoBehaviour
	: public BaseInspector, public IJsonSerializer {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////

	//* components
	using Components = Sxl::OptimizedMap<const std::type_info*, ComponentStorage::ComponentIterator>;

	//* hierarchy
	using HierarchyElement  = std::variant<MonoBehaviour*, std::unique_ptr<MonoBehaviour>>;
	using Hierarchy         = std::list<HierarchyElement>;
	using HierarchyIterator = Hierarchy::iterator;

	//* container
	using Container          = std::list<MonoBehaviour*>;
	using ContainerIterator  = Container::iterator;

	////////////////////////////////////////////////////////////////////////////////////////////
	// visitor structures
	////////////////////////////////////////////////////////////////////////////////////////////

	struct GetPtrVisitor {
		MonoBehaviour* operator()(MonoBehaviour* ptr) const { return ptr; }
		MonoBehaviour* operator()(const std::unique_ptr<MonoBehaviour>& ptr) const { return ptr.get(); }
	};

	struct ReleaseVisitor {
		void operator()(MonoBehaviour* ptr) const { ptr->RemoveParent(); }
		void operator()(std::unique_ptr<MonoBehaviour>& ptr) const { ptr.reset(); }
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	
	MonoBehaviour();
	virtual ~MonoBehaviour();

	//* flag option *//

	void SetActive(bool isActive);

	bool IsActive() const { return isActive_; }

	//* name option *//

	void SetName(const std::string& name);

	void SetRenamable(bool isRenamable) { isRenamable_ = isRenamable; }

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

	//* hierarchy option *//
	//* parent

	//! @brief parent関係を設定
	//! @param[in] parent 設定するparent
	void SetParent(MonoBehaviour* parent);

	//! @brief parent関係を削除
	void RemoveParent();

	//! @brief parentを取得
	MonoBehaviour* GetParent() const { return parent_; }

	//! @brief parentを取得
	//! @throw parentが存在しない場合に例外をスローする
	MonoBehaviour* RequireParent() const;

	//! @brief parentが存在するか確認
	//! @retval true  存在する
	//! @retval false 存在しない
	bool HasParent() const { return parent_ != nullptr; }

	//* children

	//! @brief childを追加
	//! @param[in] child 追加するchild(所有権なし)
	void AddChild(MonoBehaviour* child);

	//! @brief childを追加
	//! @param[in] child 追加するchild(所有権あり)
	void AddChild(std::unique_ptr<MonoBehaviour>&& child);

	//! @brief 名前からchildを検索
	//! @param[in] name 取得するchildの名前
	//! @retval ptr     存在する
	//! @retval nullptr 存在しない
	//! @note 自身のchildrenから探索 o(n)
	MonoBehaviour* FindChild(const std::string& name);

	//! @brief 名前からchildを検索
	//! @param[in] name 取得するchildの名前
	//! @throw childが存在しない場合に例外をスローする
	//! @note 自身のchildrenから探索 o(n)
	MonoBehaviour* FindRequireChild(const std::string& name);

	const Hierarchy& GetChildren() const { return children_; }

	//! @brief childが存在するか確認
	bool HasChild() const { return !children_.empty(); }

	//! @brief 全てのchildに対して関数を実行
	//! @param function 実行関数
	void ForEachChild(const std::function<void(MonoBehaviour*)>& function);

	//* inspector option *//

	void ShowInspector() override;

	virtual void LateUpdateInspector() override;

	//* json option *//

	json PerseToJson() const override;

	void InputJson(const json& data) override;

	void LoadComponent(const std::filesystem::path& filepath);

	void SaveComponent(const std::filesystem::path& filepath);

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* name
	std::string name_ = "new behaviour";
	bool isRenamable_ = true; //!< systemとして使われない限りtrue.

	//* flag
	bool isActive_ = true;

	//* components
	Components components_;

	//=========================================================================================
	// protected methods
	//=========================================================================================

	virtual void Inspectable() {}

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* hierarchy
	// parent information
	MonoBehaviour* parent_                     = nullptr;
	std::optional<HierarchyIterator> iterator_ = std::nullopt;

	// children information
	Hierarchy children_;

	//* container
	std::optional<ContainerIterator> containerIterator_;

	//=========================================================================================
	// private methodsWW
	//=========================================================================================

	//* hierarchy option *//

	HierarchyIterator AddHierarchy(HierarchyElement&& child);

	void RemoveHierarchy(const HierarchyIterator& iterator);

	//* behaviour container option *//

	void ApplyContainerIterator();

	//* helper *//

	static MonoBehaviour* GetElement(const HierarchyElement& child);

};

////////////////////////////////////////////////////////////////////////////////////////////
// MonoBehaviour class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <Component _Ty>
_Ty* MonoBehaviour::AddComponent() {
	// component typeの取得
	constexpr const std::type_info* type = &typeid(_Ty);

	// componentの追加
	if (!components_.Contains(type)) {
		components_[type] = sComponentStorage->RegisterComponent<_Ty>(this);
		
	} else {
		Logger::WarningRuntime("[MonoBehaviour]", "component is already added. component is only one.");
	}

	return static_cast<_Ty*>(components_[type]->get());
}

template <Component _Ty>
void MonoBehaviour::RemoveComponent() {
	// component typeの取得
	constexpr const std::type_info* type = &typeid(_Ty);

	// componentの削除
	if (components_.Contains(type)) {
		sComponentStorage->UnregisterComponent<_Ty>(components_[type]);
		components_.Erase(type);

	} else {
		Logger::WarningRuntime("[MonoBehaviour]", "component is not found. \n type: " + std::string(type->name()));
	}
}

template <Component _Ty>
const _Ty* MonoBehaviour::GetComponent() const {
	// component typeの取得
	constexpr const std::type_info* type = &typeid(_Ty);

	// componentの取得
	if (components_.Contains(type)) {
		return static_cast<const _Ty*>(components_[type]->get());
	}

	//!< componentが存在しない場合
	return nullptr;
}

template <Component _Ty>
_Ty* MonoBehaviour::GetComponent() {
	// component typeの取得
	constexpr const std::type_info* type = &typeid(_Ty);

	// componentの取得
	if (components_.Contains(type)) {
		return static_cast<_Ty*>(components_[type]->get());
	}

	//!< componentが存在しない場合
	return nullptr;
}

template <Component _Ty>
const _Ty* MonoBehaviour::RequireComponent() const {
	// component typeの取得
	constexpr const std::type_info* type = &typeid(_Ty);

	// componentの取得
	if (components_.Contains(type)) {
		return static_cast<const _Ty*>(components_[type]->get());
	}

	//!< componentが存在しない場合
	Assert(false, "component is not found. type: " + std::string(type->name()));
	return nullptr;
}

template <Component _Ty>
_Ty* MonoBehaviour::RequireComponent() {
	// component typeの取得
	constexpr const std::type_info* type = &typeid(_Ty);

	// componentの取得
	if (components_.Contains(type)) {
		return static_cast<_Ty*>(components_[type]->get());
	}

	//!< componentが存在しない場合
	Exception::Assert(false, "component is not found. type: " + std::string(type->name()));
	return nullptr;
}
