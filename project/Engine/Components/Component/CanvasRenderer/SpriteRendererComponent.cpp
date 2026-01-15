#include "SpriteRendererComponent.h"
SXAVENGER_ENGINE_USING
DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../Entity/EntityBehaviour.h"

//* engine
#include <Engine/System/UI/SxImGui.h>
#include <Engine/System/System.h>
#include <Engine/Assets/Content/ContentStorage.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SpriteRendererComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

SpriteRendererComponent::SpriteRendererComponent(EntityBehaviour* behaviour)
	: BaseComponent(behaviour) {

	input_.Create(4, 2);
	input_.GetIndex()->At(0) = { 0, 1, 2 };
	input_.GetIndex()->At(1) = { 2, 1, 3 };

	bufferUV_ = std::make_unique<ConstantBuffer<Matrix4x4>>();
	bufferUV_->Create(System::GetDxDevice());
	bufferUV_->At() = Matrix4x4::Identity();

	color_.fill(kWhite4<float>);
}

void SpriteRendererComponent::ShowComponentInspector() {

	ImGui::Checkbox("## enable", &isEnable_);

	ImGui::SameLine();

	if (ImGui::BeginCombo("## texture", texture_.GetStr().c_str())) {
		for (const auto& id : sAssetStorage->GetAssetStorage<AssetTexture>() | std::views::keys) {
			if (ImGui::Selectable(id.Serialize().c_str(), texture_ == id)) {
				texture_ = id; //!< 選択されたtextureを設定
				// todo: 2d texture以外は設定しない
			}
		}
		ImGui::EndCombo();
	}

	sContentStorage->DragAndDropTargetContentFunc<ContentTexture>([this](const std::shared_ptr<ContentTexture>& content) {
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
	bufferUV_->At() = transformUV_.ToMatrix();
	
	input_.BindIABuffer(context);
}

void SpriteRendererComponent::DrawCall(const DirectXQueueContext* context) {
	input_.DrawCall(context);
}

const RectTransformComponent* SpriteRendererComponent::GetRectTransform() const {
	return BaseComponent::GetBehaviour()->RequireComponent<RectTransformComponent>();
}

json SpriteRendererComponent::ParseToJson() const {
	json component = json::object();
	component["isEnable"]    = JsonSerializeFormatter<bool>::Serialize(isEnable_);
	component["transformUV"] = transformUV_.ParseToJson();
	component["texture"]     = texture_.Serialize();

	for (uint8_t i = 0; i < magic_enum::enum_count<VertexPoint>(); ++i) {
		component["color"][magic_enum::enum_name(static_cast<VertexPoint>(i)).data()] = JsonSerializeFormatter<Color4f>::Serialize(color_[i]);
	}

	return component;
}

void SpriteRendererComponent::InputJson(const json& data) {
	isEnable_    = JsonSerializeFormatter<bool>::Deserialize(data.at("isEnable"));
	transformUV_.InputJson(data.at("transformUV"));

	if (!data["texture"].is_null()) {
		Uuid texture = Uuid::Deserialize(JsonSerializeFormatter<std::string>::Deserialize(data.at("texture")));

		// textureのuuidが存在しない場合は, tableから読み込み

		if (!sAssetStorage->Contains<AssetTexture>(texture)) {
			const auto& filepath = sAssetStorage->GetFilepath(texture);
			sContentStorage->Import<ContentTexture>(filepath);
		}

		texture_ = texture;
	}

	for (uint8_t i = 0; i < magic_enum::enum_count<VertexPoint>(); ++i) {
		color_[i] = JsonSerializeFormatter<Color4f>::Deserialize(data.at("color").at(magic_enum::enum_name(static_cast<VertexPoint>(i)).data()));
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

	const std::shared_ptr<AssetTexture> texture = texture_.WaitRequire();

	Vector2ui size = texture->GetMetadata().size;

	SxImGui::Image(texture->GetGPUHandleSRV().ptr, ImVec2{ static_cast<float>(size.x), static_cast<float>(size.y) });
}
