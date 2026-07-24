#include "MeshRendererComponent.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../Entity/EntityBehaviour.h"

//* engine
#include <Engine/System/UI/SxImGui.h>
#include <Engine/Assets/Asset/AssetStorage.h>
#include <Engine/Assets/Content/ContentStorage.h>

////////////////////////////////////////////////////////////////////////////////////////////
// MeshRendererComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void MeshRendererComponent::ShowComponentInspector() {

	ImGui::Checkbox("enable", &isEnable_);
	SxImGui::CheckBoxFlags("cast shadow", &mask_.Get(), static_cast<uint8_t>(MeshInstanceMask::Shadow));

	SxGui::ComboEnum("mode", &mode_);

	if (ImGui::BeginCombo("mesh", mesh_.GetStr().c_str())) {
		for (const auto& [id, asset] : sAssetStorage->GetStorage<AssetMesh>()) {

			std::string label = std::format("{} # {}", asset->GetName(), id.Serialize());

			if (ImGui::Selectable(label.c_str(), mesh_ == id)) {
				mesh_ = id; //!< 選択されたmeshを設定
			}
		}
		ImGui::EndCombo();
	}

	if (ImGui::BeginCombo("material", material_.GetStr().c_str())) {
		for (const auto& [id, asset] : sAssetStorage->GetStorage<AssetMaterial>()) {
			std::string label = std::format("{} # {}", asset->GetName(), id.Serialize());

			if (ImGui::Selectable(label.c_str(), material_ == id)) {
				material_ = id; //!< 選択されたmaterialを設定
			}
		}
		ImGui::EndCombo();
	}

	SxImGui::InputScalar<uint8_t>("stencil", &stencil_);
}

const TransformComponent* MeshRendererComponent::RequireTransform() const {
	return BaseComponent::GetBehaviour()->RequireComponent<TransformComponent>();
}

json MeshRendererComponent::ParseToJson() const {
	json data = json::object();

	data["mesh"]     = mesh_.Serialize();
	data["material"] = material_.Serialize();
	data["mask"]     = mask_.Get();
	data["isEnable"] = isEnable_;
	data["stencil"]  = stencil_;
	data["mode"]     = magic_enum::enum_name(mode_);

	return data;
}

void MeshRendererComponent::InputJson(const json& data) {

	Uuid mesh     = Uuid::Deserialize(data["mesh"].get<std::string>());
	Uuid material = Uuid::Deserialize(data["material"].get<std::string>());

	// mesh, materialのuuidが存在しない場合は, tableから読み込み

	if (!sAssetStorage->Contains<AssetMesh>(mesh)) {
		const std::filesystem::path& filepath = sAssetStorage->GetLocation(mesh);
		sContentStorage->Import<ContentModel>(filepath);
	}

	if (!sAssetStorage->Contains<AssetMaterial>(material)) {
		const std::filesystem::path& filepath = sAssetStorage->GetLocation(material);
		sContentStorage->Import<ContentModel>(filepath);
	}

	mesh_     = mesh;
	material_ = material;

	mask_     = static_cast<MeshInstanceMask>(data["mask"].get<uint8_t>());
	isEnable_ = data["isEnable"].get<bool>();

	stencil_  = data["stencil"].get<uint8_t>();

	mode_ = magic_enum::enum_cast<MeshRendererCommon::Mode>(data["mode"].get<std::string>()).value();
}

bool MeshRendererComponent::IsEnable() const {
	return isEnable_ && !mesh_.Empty() && !material_.Empty();
}

std::shared_ptr<AssetMesh> MeshRendererComponent::GetMesh() const {
	return mesh_.WaitRequire();
}

std::shared_ptr<AssetMaterial> MeshRendererComponent::GetMaterial() const {
	return material_.WaitRequire();
}
