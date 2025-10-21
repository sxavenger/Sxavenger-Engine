#include "MonoBehaviour.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* entity
#include "MonoBehaviourContainer.h"

//* engine
#include <Engine/System/UI/SxImGui.h>
#include <Engine/Editor/EditorEngine.h>
#include <Engine/Editor/Editors/RenderSceneEditor.h>

//* external
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// MonoBehaviour class methods
////////////////////////////////////////////////////////////////////////////////////////////

MonoBehaviour::MonoBehaviour() {
	// containerの登録
	containerIterator_ = sMonoBehaviourContainer->AddBehaviour(this);
}

MonoBehaviour::~MonoBehaviour() {
	// parentの登録解除
	if (HasParent()) {
		RemoveParent();
	}

	// childrenの登録解除
	while (!children_.empty()) {
		static ReleaseVisitor visitor;
		std::visit(visitor, children_.back());
	}

	// componentの登録解除
	for (const auto& [type, iterator] : components_.GetMap()) {
		sComponentStorage->UnregisterComponent(type, iterator);
	}
	components_.Clear();

	// containerの登録解除
	if (containerIterator_.has_value()) {
		sMonoBehaviourContainer->RemoveBehaviour(containerIterator_.value());
	}
}

void MonoBehaviour::SetActive(bool isActive) {
	isActive_ = isActive;

	for (const auto& child : children_) {
		MonoBehaviour* ptr = GetElement(child);
		ptr->SetActive(isActive);
	}
}

void MonoBehaviour::SetName(const std::string& name) {
	if (isRenamable_) {
		name_ = name;

	} else {
		Logger::WarningRuntime("[MonoBehaviour]", "behaviour is not renamable.");
	}
}

BaseComponent* MonoBehaviour::AddComponent(const std::string& component) {

	const std::type_info* type = sComponentStorage->GetComponentInfo(component);

	// componentの追加
	if (!components_.Contains(type)) {
		components_[type] = sComponentStorage->RegisterComponent(component, this);

	} else {
		Logger::WarningRuntime("[MonoBehaviour]", "component is already added. component is only one.");
	}

	return components_[type]->get();
}

void MonoBehaviour::RemoveComponent(const std::type_info* type) {
	// componentの削除
	if (components_.Contains(type)) {
		sComponentStorage->UnregisterComponent(type, components_[type]);
		components_.Erase(type);

	} else {
		Logger::WarningRuntime("[MonoBehaviour]", "component is not found. type: " + std::string(type->name()));
	}
}

MonoBehaviour* MonoBehaviour::RequireParent() const {
	Exception::Assert(parent_ != nullptr, "parent is not found.");
	return parent_;
}

void MonoBehaviour::SetParent(MonoBehaviour* parent) {
	Exception::Assert(!HasParent(), "behaviour already have parent.");
	parent_   = parent;
	iterator_ = parent->AddHierarchy(this);
	ApplyContainerIterator();
}

void MonoBehaviour::RemoveParent() {
	if (HasParent()) {
		parent_->RemoveHierarchy(iterator_.value());
		parent_   = nullptr;
		iterator_ = std::nullopt;
		ApplyContainerIterator();
	}
}

void MonoBehaviour::AddChild(MonoBehaviour* child) {
	child->SetParent(this);
}

void MonoBehaviour::AddChild(std::unique_ptr<MonoBehaviour>&& child) {
	auto ptr = child.get();
	Exception::Assert(!ptr->HasParent(), "behaviour already have parent.");
	ptr->parent_   = this;
	ptr->iterator_ = AddHierarchy(std::move(child));
	ptr->ApplyContainerIterator();
}

MonoBehaviour* MonoBehaviour::FindChild(const std::string& name) {
	for (const auto& child : children_) {
		MonoBehaviour* ptr = MonoBehaviour::GetElement(child);

		if (ptr->GetName() == name) {
			return ptr;
		}
	}

	return nullptr;
}

MonoBehaviour* MonoBehaviour::FindRequireChild(const std::string& name) {
	MonoBehaviour* child = FindChild(name);
	Exception::Assert(child != nullptr, "child is not found. name: " + name);
	return child;
}

void MonoBehaviour::ForEachChild(const std::function<void(MonoBehaviour*)>& function) {
	for (const auto& child : children_) {
		MonoBehaviour* ptr = GetElement(child);
		function(ptr);
	}
}

void MonoBehaviour::ShowInspector() {
	ImGui::BeginDisabled(!isRenamable_); //!< 名前変更不可の場合はdisabled

	if (ImGui::Checkbox("## active", &isActive_)) {
		SetActive(isActive_);
	}

	ImGui::SameLine();

	SxImGui::InputText("## name", name_);

	ImGui::EndDisabled();

	ImGui::Separator();
	ImGui::SeparatorText("components");

	std::list<const std::type_info*> deleteComponent;

	for (const auto& [type, component] : GetComponents()) {
		ImGui::PushID(type->name());

		if (ImGui::Button(":")) {
			deleteComponent.push_back(type);
		}

		ImGui::SameLine();

		// ???: child window に変更...?
		if (ImGui::CollapsingHeader(type->name(), ImGuiTreeNodeFlags_DefaultOpen)) {
			(*component)->ShowComponentInspector();
		}

		ImGui::PopID();
	}

	for (const auto& type : deleteComponent) {
		RemoveComponent(type);
	}

	//* componentの追加
	//!< 中央に配置
	ImGui::Dummy(ImVec2(0.0f, 4.0f)); // 上にスペースを追加
	const ImVec2 size = { 160, 0 }; //!< y軸は自動調整
	ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - size.x) * 0.5f);

	if (ImGui::Button("Add Component", size)) {
		ImGui::OpenPopup("## Add Component Popup");
	}

	if (ImGui::BeginPopup("## Add Component Popup")) {
		ImGui::SeparatorText("Add Component Command");
		for (const auto& name : sComponentStorage->GetFactory() | std::views::keys) {
			if (ImGui::MenuItem(name.c_str())) {
				AddComponent(name);
			}
		}

		ImGui::EndPopup();
	}

	ImGui::SeparatorText("inspectable");
	Inspectable();

}

void MonoBehaviour::LateUpdateInspector() {
	// Manipulateの設定
	sEditorEngine->ExecuteEditorFunction<RenderSceneEditor>([&](RenderSceneEditor* editor) {
		editor->Manipulate(this);
	});

	sEditorEngine->ExecuteEditorFunction<RenderSceneEditor>([&](RenderSceneEditor* editor) {
		editor->ManipulateCanvas(this);
	});
}

json MonoBehaviour::PerseToJson() const {
	json root = json::object();

	//* properties
	root["name"]        = name_;
	root["isRenamable"] = isRenamable_;
	root["isActive"]    = isActive_;

	//* components
	json& components = root["components"] = json::array();
	for (const auto& [type, component] : components_) {
		json data         = (*component)->PerseToJson();
		data["component"] = type->name();

		components.emplace_back(data);
	}

	//* children
	json& children = root["children"] = json::array();
	for (const auto& child : children_) {
		children.emplace_back(GetElement(child)->PerseToJson());
	}

	return root;
}

void MonoBehaviour::InputJson(const json& data) {

	name_        = data.value("name", "new behaviour");
	isRenamable_ = data.value("isRenamable", true);
	isActive_    = data.value("isActive", true);

	//* components
	components_.Clear();
	for (const auto& componentData : data["components"]) {
		std::string name = componentData.value("component", "");

		if (name.empty()) {
			continue;
		}

		BaseComponent* component = AddComponent(name);
		component->InputJson(componentData);
	}

	//* children
	for (const auto& childData : data["children"]) {
		std::unique_ptr<MonoBehaviour> child = std::make_unique<MonoBehaviour>();
		MonoBehaviour* ptr = child.get();
		AddChild(std::move(child));
		ptr->InputJson(childData);
	}

}

MonoBehaviour::HierarchyIterator MonoBehaviour::AddHierarchy(HierarchyElement&& child) {
	return children_.emplace(children_.end(), std::move(child));
}

void MonoBehaviour::RemoveHierarchy(const HierarchyIterator& iterator) {
	children_.erase(iterator);
}

void MonoBehaviour::ApplyContainerIterator() {
	if (HasParent()) {
		if (containerIterator_.has_value()) {
			sMonoBehaviourContainer->RemoveBehaviour(containerIterator_.value());
			containerIterator_ = std::nullopt;
		}

	} else {
		if (!containerIterator_.has_value()) {
			containerIterator_ = sMonoBehaviourContainer->AddBehaviour(this);
		}
	}
}

MonoBehaviour* MonoBehaviour::GetElement(const HierarchyElement& child) {
	static GetPtrVisitor visitor;
	return std::visit(visitor, child);
}
