#include "SpriteRendererComponent.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../Entity/MonoBehaviour.h"

//* engine
#include <Engine/System/UI/SxImGui.h>
#include <Engine/Preview/Content/UContentStorage.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SpriteRendererComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SpriteRendererComponent::CreateBuffer() {
	input_.Create(4, 2);
	input_.GetIndex()->At(0) = { 0, 1, 2 };
	input_.GetIndex()->At(1) = { 2, 1, 3 };

	bufferUV_ = std::make_unique<DimensionBuffer<Matrix4x4>>();
	bufferUV_->Create(SxavengerSystem::GetDxDevice(), 1);
	bufferUV_->At(0) = Matrix4x4::Identity();

	color_.fill(kWhite4<float>);
}

void SpriteRendererComponent::ShowComponentInspector() {

	ImGui::Checkbox("## enable", &isEnable_);

	ImGui::SameLine();

	if (ImGui::BeginCombo("## texture", texture_.GetStr().c_str())) {
		for (const auto& id : sUAssetStorage->GetAssetStorage<UAssetTexture>() | std::views::keys) {
			if (ImGui::Selectable(id.Serialize().c_str(), texture_ == id)) {
				texture_ = id; //!< 選択されたtextureを設定
				// todo: 2d texture以外は設定しない
			}
		}
		ImGui::EndCombo();
	}

	sUContentStorage->DragAndDropTargetContentFunc<UContentTexture>([this](const std::shared_ptr<UContentTexture>& content) {
		content->WaitComplete();
		texture_ = content->GetId();
	});

	ImGui::SameLine();

	if (ImGui::Button("reset")) {
		texture_ = nullptr;
	}

	if (ImGui::ColorEdit4("color", &color_[0].r)) {
		SetColor(color_[0]);
	}

	if (ImGui::CollapsingHeader("vertex color")) {
		for (uint8_t i = 0; i < magic_enum::enum_count<VertexPoint>(); ++i) {
			ImGui::ColorEdit4(magic_enum::enum_name(static_cast<VertexPoint>(i)).data(), &color_[i].r);
		}
	}

	// textureの描画
	if (texture_.Empty()) {
		return;
	}

	ImGui::Separator();
	ImGui::Separator();

	ShowTexture();

}

void SpriteRendererComponent::SetVertexColor(const Color4f& color, VertexPoint point) {
	color_[static_cast<uint8_t>(point)] = color;
}

void SpriteRendererComponent::SetColor(const Color4f& color) {
	for (uint8_t i = 0; i < magic_enum::enum_count<VertexPoint>(); ++i) {
		SetVertexColor(color, static_cast<VertexPoint>(i));
	}
}

void SpriteRendererComponent::BindAIBuffer(const DirectXQueueContext* context) {

	// 頂点の更新
	TransferPosition();
	TransferTexcoord();
	TransferColor();

	// uv変換行列を更新
	bufferUV_->At(0) = transformUV_.ToMatrix();
	
	input_.BindIABuffer(context);
}

void SpriteRendererComponent::DrawCall(const DirectXQueueContext* context) {
	input_.DrawCall(context);
}

const RectTransformComponent* SpriteRendererComponent::GetRectTransform() const {
	return BaseComponent::GetBehaviour()->RequireComponent<RectTransformComponent>();
}

json SpriteRendererComponent::PerseToJson() const {
	json component = json::object();
	component["isEnable"]    = isEnable_;
	component["transformUV"] = transformUV_.PerseToJson();
	component["texture"]     = texture_.Serialize();

	for (uint8_t i = 0; i < magic_enum::enum_count<VertexPoint>(); ++i) {
		component["color"][magic_enum::enum_name(static_cast<VertexPoint>(i)).data()] = GeometryJsonSerializer::ToJson(color_[i]);
	}

	return component;
}

void SpriteRendererComponent::InputJson(const json& data) {
	isEnable_    = data.at("isEnable").get<bool>();
	transformUV_.InputJson(data.at("transformUV"));

	if (!data["texture"].is_null()) {
		Uuid texture = Uuid::Deserialize(data["texture"].get<std::string>());

		// textureのuuidが存在しない場合は, tableから読み込み

		if (!sUAssetStorage->Contains<UAssetTexture>(texture)) {
			const auto& filepath = sUAssetStorage->GetFilepath(texture);
			sUContentStorage->Import<UContentModel>(filepath);
		}

		texture_ = texture;
	}

	for (uint8_t i = 0; i < magic_enum::enum_count<VertexPoint>(); ++i) {
		color_[i] = GeometryJsonSerializer::JsonToColor4f(data.at("color").at(magic_enum::enum_name(static_cast<VertexPoint>(i)).data()));
	}
}

void SpriteRendererComponent::TransferPosition() {

	Vector2f leftTop     = Vector2f{ 0.0f, 0.0f };
	Vector2f rightBottom = Vector2f{ 1.0f, 1.0f };

	auto vertices = input_.GetVertex();

	vertices->At(static_cast<uint8_t>(VertexPoint::LeftTop)).position     = { leftTop.x, leftTop.y, 0.0f };
	vertices->At(static_cast<uint8_t>(VertexPoint::RightTop)).position    = { rightBottom.x, leftTop.y, 0.0f };
	vertices->At(static_cast<uint8_t>(VertexPoint::LeftBottom)).position  = { leftTop.x, rightBottom.y, 0.0f };
	vertices->At(static_cast<uint8_t>(VertexPoint::RightBottom)).position = { rightBottom.x, rightBottom.y, 0.0f };
}

void SpriteRendererComponent::TransferTexcoord() {
	Vector2f leftTop     = Vector2f{ 0.0f, 0.0f };
	Vector2f rightBottom = Vector2f{ 1.0f, 1.0f };

	auto vertices = input_.GetVertex();

	vertices->At(static_cast<uint8_t>(VertexPoint::LeftTop)).texcoord     = { leftTop.x, leftTop.y };
	vertices->At(static_cast<uint8_t>(VertexPoint::RightTop)).texcoord    = { rightBottom.x, leftTop.y };
	vertices->At(static_cast<uint8_t>(VertexPoint::LeftBottom)).texcoord  = { leftTop.x, rightBottom.y };
	vertices->At(static_cast<uint8_t>(VertexPoint::RightBottom)).texcoord = { rightBottom.x, rightBottom.y };
}

void SpriteRendererComponent::TransferColor() {
	auto vertices = input_.GetVertex();

	vertices->At(static_cast<uint8_t>(VertexPoint::LeftTop)).color     = color_[static_cast<uint8_t>(VertexPoint::LeftTop)];
	vertices->At(static_cast<uint8_t>(VertexPoint::RightTop)).color    = color_[static_cast<uint8_t>(VertexPoint::RightTop)];
	vertices->At(static_cast<uint8_t>(VertexPoint::LeftBottom)).color  = color_[static_cast<uint8_t>(VertexPoint::LeftBottom)];
	vertices->At(static_cast<uint8_t>(VertexPoint::RightBottom)).color = color_[static_cast<uint8_t>(VertexPoint::RightBottom)];
}

void SpriteRendererComponent::ShowTexture() {

	const std::shared_ptr<UAssetTexture> texture = texture_.WaitRequire();

	Vector2ui size = texture->GetMetadata().size;

	SxImGui::Image(texture->GetGPUHandleSRV().ptr, ImVec2{ static_cast<float>(size.x), static_cast<float>(size.y) });
}
