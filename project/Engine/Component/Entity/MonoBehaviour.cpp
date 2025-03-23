#include "MonoBehaviour.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* entity
#include "MonoBehaviourContainer.h"

//* engine
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
		WarningRuntime("warning | [MonoBehaviour]::SetName", "behaviour is not renamable.");
	}
}

MonoBehaviour* MonoBehaviour::RequireParent() const {
	Assert(parent_ != nullptr, "parent is not found.");
	return parent_;
}

void MonoBehaviour::SetParent(MonoBehaviour* parent) {
	Assert(!HasParent(), "behaviour already have parent.");
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
	Assert(!ptr->HasParent(), "behaviour already have parent.");
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
	Assert(child != nullptr, "child is not found. name: " + name);
	return child;
}

void MonoBehaviour::ShowInspector() {
	if (buf_.empty()) {
		buf_.resize(256);
		buf_ = name_;
	}

	ImGui::BeginDisabled(!isRenamable_); //!< 名前変更不可の場合はdisabled

	if (ImGui::InputText("## name", buf_.data(), buf_.size())) { //!< test mode
		SetName(buf_);
	}

	ImGui::EndDisabled();

	ImGui::Separator();
	ImGui::SeparatorText("components");

	for (const auto& [type, component] : GetComponents().GetMap()) {
		if (ImGui::CollapsingHeader(type->name())) {
			(*component)->ShowComponentInspector();
		}
	}

	/*ImGui::SeparatorText("json");

	if (ImGui::Button("output json")) {
		OutputJson();
	}

	ImGui::SameLine();

	if (ImGui::Button("input json")) {
		InputJson();
	}*/
}

void MonoBehaviour::LateUpdate() {
	// Manipulateの設定
	sEditorEngine->ExecuteEditorFunction<RenderSceneEditor>([&](RenderSceneEditor* editor) {
		editor->Manipulate(this);
	});

	sEditorEngine->ExecuteEditorFunction<RenderSceneEditor>([&](RenderSceneEditor* editor) {
		editor->ManipulateCanvas(this);
	});
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
