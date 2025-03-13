#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "ComponentStorage.h"

//* engine
#include <Engine/System/Utility/Logger.h>

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
class MonoBehaviour {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////

	//* component container
	using ComponentContainer = Sxl::OptimizedMap<const std::type_info*, ComponentStorage::ComponentIterator>;

	//* behaviour hierarchy
	using HierarchyElement  = std::variant<MonoBehaviour*, std::unique_ptr<MonoBehaviour>>;
	using Hierarchy         = std::list<HierarchyElement>;
	using HierarchyIterator = Hierarchy::iterator;

	//* behaviour container
	using Container          = std::list<MonoBehaviour*>;
	using ContainerIterator  = Container::iterator;

	////////////////////////////////////////////////////////////////////////////////////////////
	// struct GetPtrVisitor
	////////////////////////////////////////////////////////////////////////////////////////////
	struct GetPtrVisitor { //!< using std::visit()
		MonoBehaviour* operator()(MonoBehaviour* ptr) const { return ptr; }
		MonoBehaviour* operator()(const std::unique_ptr<MonoBehaviour>& ptr) const { return ptr.get(); }
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	MonoBehaviour() { Init(); }
	~MonoBehaviour() { Term(); }

	void Init();

	void Term();

	//* active option *//

	void SetActive(bool isActive);

	bool IsActive() const { return isActive_; }

	//* view option *//

	void SetView(bool isView);

	bool IsView() const { return isView_; }
	
	//* components option *//

	template <Component T>
	T* AddComponent();

	template <Component T>
	void RemoveComponent();

	template <Component T>
	T* GetComponent() const;

	template <Component T>
	T* RequireComponent() const;

	ComponentContainer& GetComponents() { return components_; }

	//* hierarchy option *//

	void SetParent(MonoBehaviour* parent);

	void RemoveParent();

	void AddChild(MonoBehaviour* child);

	void AddChild(std::unique_ptr<MonoBehaviour>&& child);

	void RemoveChild(MonoBehaviour* child);

	MonoBehaviour* GetChild(const std::string& name);

	bool HasParent() const { return parent_ != nullptr; }

	MonoBehaviour* GetParent() const;

	MonoBehaviour* RequireParent() const;

	const Hierarchy& GetChildren() const { return children_; }

	//* name option *//

	void SetName(const std::string& name) { name_ = name; }

	const std::string& GetName() const { return name_; }

	void SetRenamable(bool isRenamable) { isRenamable_ = isRenamable; }

	//* update option *//

	void UpdateComponent();

	//* debug option *//

	void SetBehaviourImGuiCommand(char buf[256]);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* active
	bool isActive_ = true;

	//* view
	bool isView_ = true;

	//* component container
	ComponentContainer components_;

	//* hierarchy
	MonoBehaviour* parent_            = nullptr;
	std::optional<HierarchyIterator> iterator_ = std::nullopt; //!< parentのiterator.

	Hierarchy children_;

	//* name
	std::string name_ = "new behaviour";
	bool isRenamable_ = true;

	//* container
	std::optional<ContainerIterator> containerIterator_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	HierarchyIterator AddHierarchy(std::variant<MonoBehaviour*, std::unique_ptr<MonoBehaviour>>&& child);

	void RemoveHierarchy(const HierarchyIterator& iterator);

	void ApplyContainerIterator();

};

////////////////////////////////////////////////////////////////////////////////////////////
// MonoBehaviour class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <Component T>
T* MonoBehaviour::AddComponent() {

	constexpr const std::type_info* type = &typeid(T);

	if (!components_.Contains(type)) {
		components_[type] = sComponentStorage->RegisterComponent<T>(this);

	} else {
		CommentRuntime("comment | [MonoBehaviour]::AddComponent", "component is already added. type: " + std::string(type->name()));
	}

	return static_cast<T*>(components_[type]->get());
}

template <Component T>
void MonoBehaviour::RemoveComponent() {

	constexpr const std::type_info* type = &typeid(T);

	if (components_.Contains(type)) {
		sComponentStorage->UnregisterComponent<T>(components_[type]);
		components_.Erase(type);
	} else {
		CommentRuntime("comment | [MonoBehaviour]::RemoveComponent", "component is not found. type: " + std::string(type->name()));
	}
}

template <Component T>
T* MonoBehaviour::GetComponent() const {

	constexpr const std::type_info* type = &typeid(T);

	if (components_.Contains(type)) {
		return static_cast<T*>(components_.At(type)->get());
	}

	return nullptr;
}

template <Component T>
T* MonoBehaviour::RequireComponent() const {
	T* component = GetComponent<T>();

	Assert(component != nullptr, "component is not found.", "type: " + std::string(typeid(T).name()));
	return component;
}
