#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "ComponentStorage.h"

//* engine
#include <Engine/System/Utility/Logger.h>

//* c++
#include <typeindex>
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
	using ComponentContainer = std::unordered_map<std::type_index, ComponentStorage::ComponentIterator>;

	//* behaviour hierarchy
	using Hierarchy         = std::list<std::variant<MonoBehaviour*, std::unique_ptr<MonoBehaviour>>>;
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

	void SetActive(bool isActive) { isActive_ = isActive; }

	bool IsActive() const { return isActive_; }
	
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

	bool HasParent() const { return parent_ != nullptr; }

	MonoBehaviour* GetParent() const { return parent_; }

	const Hierarchy& GetChildren() const { return children_; }

	//* name option *//

	void SetName(const std::string& name) { name_ = name; }

	const std::string& GetName() const { return name_; }

	//* update option *//

	void UpdateComponent();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* active
	bool isActive_ = true;

	//* component container
	ComponentContainer components_;

	//* hierarchy
	MonoBehaviour* parent_            = nullptr;
	std::optional<HierarchyIterator> iterator_ = std::nullopt; //!< parentのiterator.

	Hierarchy children_;

	//* name
	std::string name_ = "new behaviour";

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

	std::type_index type = typeid(T);

	if (!components_.contains(type)) {
		components_[type] = sComponentStorage->RegisterComponent<T>(this);
	}

	return static_cast<T*>(components_[type]->get());
}

template <Component T>
void MonoBehaviour::RemoveComponent() {

	std::type_index type = typeid(T);

	if (components_.contains(type)) {
		sComponentStorage->UnregisterComponent<T>(components_[type]);
		components_.erase(type);
	}
}

template <Component T>
T* MonoBehaviour::GetComponent() const {

	std::type_index type = typeid(T);

	if (components_.contains(type)) {
		return static_cast<T*>(components_.at(type)->get());
	}

	return nullptr;
}

template <Component T>
T* MonoBehaviour::RequireComponent() const {
	T* component = GetComponent<T>();

	Assert(component != nullptr, "component is not found.", "type: " + std::string(typeid(T).name()));
	return component;
}
