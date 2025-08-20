#include "MeshRendererComponent.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../Entity/MonoBehaviour.h"

//* engine
#include <Engine/System/UI/SxImGui.h>
#include <Engine/Preview/Asset/UAssetStorage.h>

////////////////////////////////////////////////////////////////////////////////////////////
// MeshRendererComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void MeshRendererComponent::ShowComponentInspector() {

	SxImGui::CheckBoxFlags("cast shadow", &mask_.Get(), static_cast<uint8_t>(MeshInstanceMask::Shadow));

	/*if (ImGui::TreeNodeEx("material", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_NoAutoOpenOnLog | ImGuiTreeNodeFlags_NoTreePushOnOpen)) {
		if (material_ != nullptr) {
			material_->SetImGuiCommand();

		} else {
			ImGui::Text("material is nullptr.");
		}
	}*/
}

const TransformComponent* MeshRendererComponent::GetTransform() const {
	return BaseComponent::GetBehaviour()->RequireComponent<TransformComponent>(); //!< TransformComponentが必要
}

json MeshRendererComponent::PerseToJson() const {
	json data = json::object();

	data["mesh"]     = mesh_.Serialize();
	data["material"] = material_.Serialize();
	data["mask"]     = mask_.Get();

	return data;
}

void MeshRendererComponent::InputJson(const json& data) {
	data;
}

std::shared_ptr<UAssetMesh> MeshRendererComponent::GetMesh() const {
	return mesh_.Require();
}

std::shared_ptr<UAssetMaterial> MeshRendererComponent::GetMaterial() const {
	return material_.Require();
}
