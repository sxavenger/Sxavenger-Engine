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
	: public BaseInspector {
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

	void SetView(bool isView);

	bool IsView() const { return isView_; }

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

	//! @brief componentを削除
	//! @tparam _Ty 削除するcomponentの型
	template <Component _Ty>
	void RemoveComponent();

	//! @brief componentを取得
	//! @tparam _Ty 取得するcomponentの型
	//! @retval componentのptr, 存在しない場合はnullptr
	template <Component _Ty>
	const _Ty* GetComponent() const;

	//! @brief componentを取得
	//! @tparam _Ty 取得するcomponentの型
	//! @retval componentのptr, 存在しない場合はnullptr
	template <Component _Ty>
	_Ty* GetComponent();

	//! @brief componentを取得
	//! @tparam _Ty 取得するcomponentの型
	//! @return componentのptr(存在しない場合はAssert)
	template <Component _Ty>
	const _Ty* RequireComponent() const;

	//! @brief componentを取得
	//! @tparam _Ty 取得するcomponentの型
	//! @return componentのptr(存在しない場合はAssert)
	template <Component _Ty>
	_Ty* RequireComponent();

	const Components& GetComponents() const { return components_; }
	Components& GetComponents() { return components_; }

	//* hierarchy option *//
	//* parent

	void SetParent(MonoBehaviour* parent);

	void RemoveParent();

	MonoBehaviour* GetParent() const { return parent_; }

	MonoBehaviour* RequireParent() const;

	bool HasParent() const { return parent_ != nullptr; }

	//* children

	void AddChild(MonoBehaviour* child);

	void AddChild(std::unique_ptr<MonoBehaviour>&& child);

	MonoBehaviour* FindChild(const std::string& name);

	MonoBehaviour* FindRequireChild(const std::string& name);

	const Hierarchy& GetChildren() const { return children_; }

	//* inspector option *//

	virtual void ShowInspector() override;

	virtual void LateUpdate() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* name
	std::string name_ = "new behaviour";
	bool isRenamable_ = true; //!< systemとして使われない限りtrue.

	std::string buf_ = "";

	//* flag
	bool isActive_ = true;
	bool isView_   = true;

	//* components
	Components components_;

	//* hierarchy
	// parent information
	MonoBehaviour* parent_                     = nullptr;
	std::optional<HierarchyIterator> iterator_ = std::nullopt;

	// children information
	Hierarchy children_;

	//* container
	std::optional<ContainerIterator> containerIterator_;

	//=========================================================================================
	// private methods
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
		WarningRuntime("warning | [MonoBehaviour]::AddComponent", "component is already added. component is only one.");
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
		WarningRuntime("warning | [MonoBehaviour]::RemoveComponent", "component is not found. type: " + std::string(type->name()));
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
	Assert(false, "component is not found. type: " + std::string(type->name()));
	return nullptr;
}
