#include "DecalRendererComponent.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../Entity/EntityBehaviour.h"

//* engine
#include <Engine/Graphics/Graphics.h>
#include <Engine/Assets/Asset/AssetStorage.h>
#include <Engine/Assets/Content/ContentStorage.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DecalRendererComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DecalRendererComponent::ShowComponentInspector() {

	if (ImGui::BeginCombo("texture", texture_.GetStr().c_str())) {
		for (const auto& [id, asset] : sAssetStorage->GetStorage<AssetTexture>()) {

			std::string label = std::format("{} # {}", asset->GetName(), id.Serialize());

			if (ImGui::Selectable(label.c_str(), texture_ == id)) {
				texture_ = id; //!< 選択されたtextureを設定
			}
		}

		ImGui::EndCombo();
	}

	sContentStorage->DragAndDropTargetContentFunc<ContentTexture>([this](const std::shared_ptr<ContentTexture>& content) {
		content->WaitComplete();
		texture_ = content->GetId();
	});

	PushDecalLine();
}

const TransformComponent* DecalRendererComponent::GetTransform() const {
	return BaseComponent::GetBehaviour()->GetComponent<TransformComponent>();
}

const TransformComponent* DecalRendererComponent::RequireTransform() const {
	return BaseComponent::GetBehaviour()->RequireComponent<TransformComponent>();
}

json DecalRendererComponent::ParseToJson() const {
	json data = json::object();

	data["texture"] = texture_.Serialize();

	return data;
}

void DecalRendererComponent::InputJson(const json& data) {

	Uuid texture = Uuid::Deserialize(data["texture"].get<std::string>());

	if (!sAssetStorage->Contains<AssetTexture>(texture)) {
		//!< textureのuuidが存在しない場合は, tableから読み込み
		const std::filesystem::path& filepath = sAssetStorage->GetLocation(texture);
		sContentStorage->Import<ContentTexture>(filepath);
	}

	texture_ = texture;
	
}

void DecalRendererComponent::PushDecalLine() const {

	auto transform = BaseComponent::GetBehaviour()->GetComponent<TransformComponent>();

	if (transform == nullptr) {
		return;
	}

	static const std::array<Vector3f, 8> kVertices = {
		Vector3f{ -0.5f, -0.5f, -0.5f },
		Vector3f{  0.5f, -0.5f, -0.5f },
		Vector3f{  0.5f,  0.5f, -0.5f },
		Vector3f{ -0.5f,  0.5f, -0.5f },

		Vector3f{ -0.5f, -0.5f,  0.5f },
		Vector3f{  0.5f, -0.5f,  0.5f },
		Vector3f{  0.5f,  0.5f,  0.5f },
		Vector3f{ -0.5f,  0.5f,  0.5f },
	};

	static const uint32_t kEdges[][2] = {
		{ 0, 1 },
		{ 1, 2 },
		{ 2, 3 },
		{ 3, 0 },
		{ 4, 5 },
		{ 5, 6 },
		{ 6, 7 },
		{ 7, 4 },
		{ 0, 4 },
		{ 1, 5 },
		{ 2, 6 },
		{ 3, 7 },
	};

	std::array<Vector3f, 8> vertices = {};
	const Matrix4x4& world = transform->GetMatrix();

	for (uint32_t i = 0; i < 8; ++i) {
		vertices[i] = Matrix4x4::Transform(kVertices[i], world);
	}

	for (const auto& edge : kEdges) {
		Graphics::PushLine(vertices[edge[0]], vertices[edge[1]], Color4f::Convert(0x18962CFF), 0.6f);
	}

	Graphics::PushLine(transform->GetPosition(), transform->GetPosition() + transform->GetDirection(), Color4f::Convert(0xFFFFFFFF), 1.0f);

}

