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

void MonoBehaviour::SetView(bool isView) {
	isView_ = isView;

	for (const auto& child : children_) {
		MonoBehaviour* ptr = GetElement(child);
		ptr->SetView(isView);
	}
}

void MonoBehaviour::SetName(const std::string& name) {
	if (isRenamable_) {
		name_ = name;

	} else {
		Logger::WarningRuntime("warning | [MonoBehaviour]::SetName", "behaviour is not renamable.");
	}
}

BaseComponent* MonoBehaviour::AddComponent(const std::string& component) {

	const std::type_info* type = sComponentStorage->GetComponentInfo(component);

	// componentの追加
	if (!components_.Contains(type)) {
		components_[type] = sComponentStorage->RegisterComponent(component, this);

	} else {
		Logger::WarningRuntime("warning | [MonoBehaviour]::AddComponent", "component is already added. component is only one.");
	}

	return components_[type]->get();
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

void MonoBehaviour::ShowInspector() {
	if (buf_.empty()) {
		buf_ = name_;
		buf_.resize(128);
	}

	ImGui::BeginDisabled(!isRenamable_); //!< 名前変更不可の場合はdisabled

	if (ImGui::Checkbox("## active", &isActive_)) {
		SetActive(isActive_);
	}

	ImGui::SameLine();

	SxImGui::InputTextFunc("## name", buf_, [this](const std::string& name) {
		SetName(name);
	});

	ImGui::EndDisabled();

	ImGui::Separator();
	ImGui::SeparatorText("components");

	for (const auto& [type, component] : GetComponents()) {
		if (ImGui::CollapsingHeader(type->name(), ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::PushID(type->name());
			(*component)->ShowComponentInspector();
			ImGui::PopID();
		}
	}

	ImGui::SeparatorText("inspectable");
	Inspectable();

	/*ImGui::SeparatorText("preview");
	if (ImGui::Button("prese")) {
		std::filesystem::path filepath = name_ + ".json";
		JsonHandler::WriteToJson(filepath, PerseToJson());
	}*/

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
	root["isView"]      = isView_;

	//* components
	json& components = root["components"] = json::array();
	for (const auto& component : components_ | std::views::values) {
		components.emplace_back((*component)->PerseToJson());
	}

	//* children
	json& children = root["children"] = json::array();
	for (const auto& child : children_) {
		children.emplace_back(GetElement(child)->PerseToJson());
	}

	return root;
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
