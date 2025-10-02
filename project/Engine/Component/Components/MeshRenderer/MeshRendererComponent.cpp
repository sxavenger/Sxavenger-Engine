#include "MeshRendererComponent.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../Entity/MonoBehaviour.h"

//* engine
#include <Engine/System/UI/SxImGui.h>
#include <Engine/Preview/Asset/UAssetStorage.h>
#include <Engine/Preview/Content/UContentStorage.h>

////////////////////////////////////////////////////////////////////////////////////////////
// MeshRendererComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void MeshRendererComponent::ShowComponentInspector() {

	ImGui::Checkbox("enable", &isEnable_);
	SxImGui::CheckBoxFlags("cast shadow", &mask_.Get(), static_cast<uint8_t>(MeshInstanceMask::Shadow));

	if (ImGui::BeginCombo("mesh", mesh_.GetStr().c_str())) {
		for (const auto& id : sUAssetStorage->GetAssetStorage<UAssetMesh>() | std::views::keys) {
			if (ImGui::Selectable(id.Serialize().c_str(), mesh_ == id)) {
				mesh_ = id; //!< 選択されたmeshを設定
			}
		}
		ImGui::EndCombo();
	}

	if (ImGui::BeginCombo("material", material_.GetStr().c_str())) {
		for (const auto& id : sUAssetStorage->GetAssetStorage<UAssetMaterial>() | std::views::keys) {
			if (ImGui::Selectable(id.Serialize().c_str(), material_ == id)) {
				material_ = id; //!< 選択されたmaterialを設定
			}
		}
		ImGui::EndCombo();
	}
}

const TransformComponent* MeshRendererComponent::GetTransform() const {
	return BaseComponent::GetBehaviour()->RequireComponent<TransformComponent>(); //!< TransformComponentが必要
}

json MeshRendererComponent::PerseToJson() const {
	json data = json::object();

	data["mesh"]     = mesh_.Serialize();
	data["material"] = material_.Serialize();
	data["mask"]     = mask_.Get();
	data["isEnable"] = isEnable_;

	return data;
}

void MeshRendererComponent::InputJson(const json& data) {

	Uuid mesh     = Uuid::Deserialize(data["mesh"].get<std::string>());
	Uuid material = Uuid::Deserialize(data["material"].get<std::string>());

	// mesh, materialのuuidが存在しない場合は, tableから読み込み

	if (!sUAssetStorage->Contains<UAssetMesh>(mesh)) {
		const auto& filepath = sUAssetStorage->GetFilepath(mesh);
		sUContentStorage->Import<UContentModel>(filepath);
	}

	if (!sUAssetStorage->Contains<UAssetMaterial>(material)) {
		const auto& filepath = sUAssetStorage->GetFilepath(material);
		sUContentStorage->Import<UContentModel>(filepath);
	}

	mesh_     = mesh;
	material_ = material;

	mask_     = static_cast<MeshInstanceMask>(data["mask"].get<uint8_t>());
	isEnable_ = data["isEnable"].get<bool>();
}

std::shared_ptr<UAssetMesh> MeshRendererComponent::GetMesh() const {
	return mesh_.Require();
}

std::shared_ptr<UAssetMaterial> MeshRendererComponent::GetMaterial() const {
	return material_.Require();
}
