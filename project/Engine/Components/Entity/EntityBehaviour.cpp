#include "EntityBehaviour.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* entity
#include "EntityBehaviourStorage.h"
#include "BehaviourHelper.h"

//* engine
#include <Engine/System/UI/SxImGui.h>
#include <Engine/System/WinApp/WinApp.h>
#include <Engine/Editors/EditorEngine.h>
#include <Engine/Editors/Editor/RenderSceneEditor.h>

////////////////////////////////////////////////////////////////////////////////////////////
// EntityBehaviour class methods
////////////////////////////////////////////////////////////////////////////////////////////

EntityBehaviour::EntityBehaviour() = default;

EntityBehaviour::~EntityBehaviour() {

	//!< parentの登録解除
	if (HasParent()) {
		parent_->RemoveChild(this);
	}

	//!< childrenの登録解除
	for (const auto& child : children_) {
		child->RemoveParent(this);

		if (child == BehaviourAddress::Ownership::Borrowed) {
			continue;
		}

		//!< 特殊処理: 後にEntityBehaviourStorageで解放されるため、所有権を渡す
		BehaviourAddress address = { child.GetAddress(), BehaviourAddress::Ownership::Owned };
		sEntityBehaviourStorage->PushUnregisterQueue(address);
	}

	//!< componentの登録解除
	for (const auto& [type, iterator] : components_) {
		sComponentStorage->UnregisterComponent(type, iterator);
	}
	
}

void EntityBehaviour::SetActive(bool isActive) {
	isActive_ = isActive;

	for (const auto& child : children_) {
		child->SetActive(isActive);
	}
}

void EntityBehaviour::SetRenamable(bool isRenamable) {
	isRenamable_ = isRenamable;
}

void EntityBehaviour::SetName(const std::string& name) {
	if (!isRenamable_) {
		StreamLogger::EngineLog("[EntityBehaviour] warning | behaviour is not renamable.");
		return;
	}

	name_ = name;
}

BaseComponent* EntityBehaviour::AddComponent(const std::string& component) {

	const std::type_info* type = sComponentStorage->GetComponentInfo(component);

	// componentの追加
	if (!components_.contains(type)) {
		components_[type] = sComponentStorage->RegisterComponent(component, this);

	} else {
		RuntimeLogger::LogWarning("[EntityBehaviour]", "component is already added. component is only one.");
	}

	return components_[type]->get();
}

void EntityBehaviour::RemoveComponent(const std::type_info* type) {
	// componentの削除
	if (!components_.contains(type)) {
		RuntimeLogger::LogWarning("[EntityBehaviour]", "component is not found. type: " + std::string(type->name()));
		return;
	}

	sComponentStorage->UnregisterComponent(type, components_[type]);
	components_.erase(type);
}

void EntityBehaviour::AddChild(const BehaviourAddress& child) {
	StreamLogger::AssertA(child != nullptr, "behaviour address is nullptr.");
	child->SetParent(this);
	children_.emplace(child);
}

void EntityBehaviour::AddChild(BehaviourAddress&& child) {
	StreamLogger::AssertA(child != nullptr, "behaviour address is nullptr.");
	child->SetParent(this);
	children_.emplace(std::move(child));
}

void EntityBehaviour::RemoveChild(const BehaviourAddress& child) {
	StreamLogger::AssertA(children_.contains(child), "child behaviour not found.");
	child->RemoveParent(this);
	children_.erase(child);
}

BehaviourAddress EntityBehaviour::FindChild(const std::string& name) const {
	// FIXME: o(n)なのでmapなどに変更検討

	for (const auto& child : children_) {
		if (child->GetName() == name) {
			return child;
		}
	}

	return nullptr;
}

const BehaviourAddress& EntityBehaviour::GetParent() const {
	return parent_;
}

const BehaviourAddress& EntityBehaviour::RequireParent() const {
	StreamLogger::AssertA(HasParent(), "parent behaviour is empty.");
	return parent_;
}

void EntityBehaviour::ForEachChild(const std::function<void(EntityBehaviour*)>& function) const {
	for (const auto& child : children_) {
		function(child.Get());
		child->ForEachChild(function);
	}
}

void EntityBehaviour::ShowInspector() {

	ImGui::BeginDisabled(!isRenamable_); //!< 名前変更不可の場合はdisabled

	if (ImGui::Checkbox("## active", &isActive_)) {
		SetActive(isActive_);
	}

	ImGui::SameLine();

	SxImGui::InputText("## name", name_);

	ImGui::EndDisabled();

	ImGui::Text(std::format("mobility - {}", magic_enum::enum_name(mobility_)).c_str());

	{ //!< hierarchyの表示
		ImGui::BeginChild("## hierarchy child", ImVec2(0, 120), ImGuiChildFlags_ResizeY);

		if (SxGui::Hierarchy::Begin()) {
			HierarchyTreeNode(this);
			SxGui::Hierarchy::End();
		}

		ImGui::EndChild();
	}

	{ //!< componentの表示・追加・削除
		ImGui::SeparatorText("components");

		std::queue<const std::type_info*> unregister;

		for (const auto& [type, component] : GetComponents()) {
			ImGui::PushID(type->name());

			if (ImGui::Button(":")) {
				unregister.emplace(type);
			}

			ImGui::SameLine();

			// ???: child window に変更...?
			if (ImGui::CollapsingHeader(type->name(), ImGuiTreeNodeFlags_DefaultOpen)) {
				(*component)->ShowComponentInspector();
			}

			ImGui::PopID();
		}

		while (!unregister.empty()) {
			RemoveComponent(unregister.front());
			unregister.pop();
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
	}
	
	{ //!< inspectableの表示
		ImGui::SeparatorText("inspectable");
		if (inspectable_ != nullptr) {
			inspectable_(this);
		}
	}

	ImGui::Separator();

	if (ImGui::Button("Load")) {
		auto filepath = WinApp::GetOpenFilepath(L"behaviourを選択", std::filesystem::current_path(), { L"behaviourファイル", L"*.behaviour" });

		if (filepath.has_value()) {
			LoadComponent(filepath.value());
		}
	}

	ImGui::SameLine();

	if (ImGui::Button("Save")) {
		auto filepath = WinApp::GetSaveFilepath(L"behaviourを保存", std::filesystem::current_path(), { L"behaviourファイル", L"*.behaviour" }, ".behaviour");

		if (filepath.has_value()) {
			SaveComponent(filepath.value());
		}
	}

	ImGui::SameLine();

	if (ImGui::Button("Delete Behaviour")) {
		sEntityBehaviourStorage->PushUnregisterQueue(this->GetAddress());
	}
}

void EntityBehaviour::LateUpdateInspector() {
	// Manipulateの設定
	sEditorEngine->ExecuteEditorFunction<RenderSceneEditor>([&](RenderSceneEditor* editor) {
		editor->Manipulate(this);
	});

	sEditorEngine->ExecuteEditorFunction<RenderSceneEditor>([&](RenderSceneEditor* editor) {
		editor->ManipulateCanvas(this);
	});
}

json EntityBehaviour::ParseToJson() const {
	json root = json::object();

	//* properties
	root["name"]        = name_;
	root["isRenamable"] = isRenamable_;
	root["isActive"]    = isActive_;

	//* components
	json& components = root["components"] = json::array();
	for (const auto& [type, component] : components_) {
		json data         = (*component)->ParseToJson();
		data["component"] = type->name();

		components.emplace_back(data);
	}

	//* children
	json& children = root["children"] = json::array();
	for (const auto& child : children_) {
		children.emplace_back(child->ParseToJson());
	}

	return root;
}

void EntityBehaviour::InputJson(const json& data) {

	name_        = data.value("name", "new behaviour");
	isRenamable_ = data.value("isRenamable", true);
	isActive_    = data.value("isActive", true);

	//* components
	components_.clear();
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

		BehaviourAddress address = sEntityBehaviourStorage->RegisterBehaviour();
		EntityBehaviour* ptr = address.Get();
		AddChild(std::move(address));
		ptr->InputJson(childData);
	}

}

void EntityBehaviour::LoadComponent(const std::filesystem::path& filepath) {
	json data;
	if (JsonHandler::LoadFromJson(filepath, data)) {
		InputJson(data);
	}
}

void EntityBehaviour::SaveComponent(const std::filesystem::path& filepath) {
	json data = ParseToJson();
	JsonHandler::WriteToJson(filepath, data);
}

void EntityBehaviour::SetParent(EntityBehaviour* parent) {
	StreamLogger::AssertA(!HasParent(), "behaviour already have parent.");
	parent_ = BehaviourAddress(parent->GetAddress(), BehaviourAddress::Ownership::Borrowed);
}

void EntityBehaviour::RemoveParent(const EntityBehaviour* parent) {
	StreamLogger::AssertA(parent_ == parent->GetAddress(), "parent behaviour mismatch.");
	parent_ = nullptr;
}

void EntityBehaviour::RemoveChild(EntityBehaviour* child) {
	StreamLogger::AssertA(children_.contains(child->GetAddress()), "child behaviour not found.");
	child->RemoveParent(this);
	children_.erase(child->GetAddress());
}

void EntityBehaviour::HierarchyTreeNode(EntityBehaviour* behaviour) {

	bool isInspector  = behaviour->CheckInspector();
	std::string label = std::format("{} # 0x{:x}", GetName(), GetAddress());

	if (!behaviour->IsActive()) {
		ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
	}

	bool isOpen = SxGui::Hierarchy::TreeNode(label.c_str(), isInspector, !HasChild(), ImGuiTreeNodeFlags_DefaultOpen);

	if (!behaviour->IsActive()) {
		ImGui::PopStyleColor();
	}

	//!< 選択処理
	if (ImGui::IsItemClicked()) {
		behaviour->SetInspector();
	}

	if (isOpen) {
		for (const auto& child : behaviour->children_) {
			child->HierarchyTreeNode(child.Get());
		}

		SxGui::Hierarchy::TreePop();
	}

}
