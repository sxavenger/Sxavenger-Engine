#include "MonoBehaviour.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "MonoBehaviourContainer.h"
#include "Components/Transform/TransformComponent.h"
#include "Components/Camera/CameraComponent.h"

//* external
#include <imgui.h>

//* c++
#include <execution>

////////////////////////////////////////////////////////////////////////////////////////////
// MonoBehaviour class methods
////////////////////////////////////////////////////////////////////////////////////////////

void MonoBehaviour::Init() {
	// containerの登録
	containerIterator_ = sMonoBehaviourContainer->AddBehaviour(this);
}

void MonoBehaviour::Term() {
	// parentの登録解除
	RemoveParent();

	struct TermVisit {
		void operator()(MonoBehaviour* behavior) const { behavior->Term(); }
		void operator()(std::unique_ptr<MonoBehaviour>& behavior) const { behavior.reset(); }
	};

	// childrenの登録解除
	while (!children_.empty()) {
		std::visit(TermVisit{}, children_.back());
	}

	// componentの登録解除
	std::for_each(std::execution::seq, components_.GetMap().begin(), components_.GetMap().end(), [](const std::pair<std::type_index, ComponentStorage::ComponentIterator>& pair) {
		const auto& [type, iterator] = pair;
		sComponentStorage->UnregisterComponent(type, iterator);
	});
	components_.Clear();

	// containerの登録解除
	if (containerIterator_.has_value()) {
		sMonoBehaviourContainer->RemoveBehaviour(containerIterator_.value());
	}
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

void MonoBehaviour::RemoveChild(MonoBehaviour* child) {
	child->RemoveParent();
}

MonoBehaviour* MonoBehaviour::GetChild(const std::string& name) {
	for (const auto& child : children_) {
		MonoBehaviour* ptr = std::visit(GetPtrVisitor{}, child);

		if (ptr->GetName() == name) {
			return ptr;
		}
	}
}

void MonoBehaviour::UpdateComponent() {
	if (auto transform = GetComponent<TransformComponent>()) {
		transform->UpdateMatrix();
	}

	if (auto camera = GetComponent<CameraComponent>()) {
		camera->UpdateView();
		camera->UpdateProj();
	}
}

void MonoBehaviour::SetBehaviourImGuiCommand(char buf[256]) {
	ImGui::Checkbox("## Active", &isActive_);
	ImGui::SameLine();

	if (ImGui::InputText("## Name", buf, 256)) {
		SetName(buf);
	}

	ImGui::Separator();

	for (const auto& [type, component] : GetComponents().GetMap()) {
		if (ImGui::CollapsingHeader(type.name())) {
			(*component)->InspectorImGui();
		}
	}

}

MonoBehaviour::HierarchyIterator MonoBehaviour::AddHierarchy(std::variant<MonoBehaviour*, std::unique_ptr<MonoBehaviour>>&& child) {
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
