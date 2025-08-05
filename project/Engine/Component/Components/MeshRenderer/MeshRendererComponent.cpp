#include "MeshRendererComponent.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../Entity/MonoBehaviour.h"

////////////////////////////////////////////////////////////////////////////////////////////
// MeshRendererComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void MeshRendererComponent::ShowComponentInspector() {

	if (ImGui::TreeNodeEx("material", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_NoAutoOpenOnLog | ImGuiTreeNodeFlags_NoTreePushOnOpen)) {
		if (material_ != nullptr) {
			material_->SetImGuiCommand();

		} else {
			ImGui::Text("material is nullptr.");
		}
	}
}

const TransformComponent* MeshRendererComponent::GetTransform() const {
	if (transform_ != nullptr) {
		return transform_;
	}

	return BaseComponent::GetBehaviour()->RequireComponent<TransformComponent>(); //!< TransformComponentが必要
}
