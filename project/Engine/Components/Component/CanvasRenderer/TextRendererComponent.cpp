#include "TextRendererComponent.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../Entity/MonoBehaviour.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/Utility/Convert.h>
#include <Engine/System/Utility/RuntimeLogger.h>
#include <Engine/System/UI/SxImGui.h>
#include <Engine/Assets/Content/ContentStorage.h>
#include <Engine/Assets/Asset/AssetStorage.h>

////////////////////////////////////////////////////////////////////////////////////////////
// TextRendererComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

TextRendererComponent::TextRendererComponent(MonoBehaviour* behaviour)
	: BaseComponent(behaviour) {

	input_.Create(static_cast<uint32_t>(kMaxTextLength * 4), static_cast<uint32_t>(kMaxTextLength * 2));

	auto vertices = input_.GetVertex();
	auto indices  = input_.GetIndex();

	// vertexの初期化
	vertices->Fill(CanvasVertexData{});

	// indexの初期化
	for (size_t i = 0; i < kMaxTextLength; ++i) {
		indices->At(i * 2 + 0) = { static_cast<UINT>(i * 4 + 0), static_cast<UINT>(i * 4 + 1), static_cast<UINT>(i * 4 + 2) };
		indices->At(i * 2 + 1) = { static_cast<UINT>(i * 4 + 0), static_cast<UINT>(i * 4 + 2), static_cast<UINT>(i * 4 + 3) };
	}
}

void TextRendererComponent::ShowComponentInspector() {
	SxImGui::InputTextFunc("## Text Box", ToString(text_), [this](const std::string& str) {
		SetText(ToWString(str));
	});
	// TODO: MultilineInputTextFuncを使用する

	if (ImGui::BeginCombo("font", font_.GetStr().c_str())) {
		for (const auto& id : sAssetStorage->GetAssetStorage<AssetFont>() | std::views::keys) {
			if (ImGui::Selectable(id.Serialize().c_str(), font_ == id)) {
				font_ = id; //!< 選択されたfontを設定
			}
		}
		ImGui::EndCombo();
	}

	sContentStorage->DragAndDropTargetContentFunc<ContentFont>([this](const std::shared_ptr<ContentFont>& content) {
		content->WaitComplete();
		font_ = content->GetId();
	});

	ImGui::ColorEdit4("color", &color_.r);
	SxImGui::DragScalarN<float, 1>("size", &size_, 0.1f, 0.0f, 100.0f, "%.1f");
}

void TextRendererComponent::SetText(const std::wstring& text) {
	text_ = text.substr(0, kMaxTextLength - 1);
}

void TextRendererComponent::PerseText() {
	if (text_.empty()) {
		return;
	}

	if (font_.Empty()) {
		RuntimeLogger::LogWarning("[TextRendererComponent]", "font is not set.");
		return;
	}

	// fontの取得
	const std::shared_ptr<AssetFont> font = font_.WaitRequire();

	const RectTransformComponent* component = GetRectTransform();
	Vector2f scale = component->GetTransform().scale;

	Vector2f cursor = { 0, font->GetFontSize() };
	size_t index    = 0;

	auto vertices = input_.GetVertex();

	// vertexの初期化
	vertices->Fill(CanvasVertexData{});

	// textの解析
	for (wchar_t c : text_) {
		if (c == L'\0') { //!< 終端文字
			break;
		}

		if (c == L'\n') { //!< 改行
			cursor.x = 0;
			cursor.y += font->GetFontSize();
			continue;
		}

		if (c == L' ') { //!< 空白
			cursor.x += font->GetFontSize() * 0.3f; //!< スペース幅
			continue;
		}

		const AssetFont::GlyphInfo& glyph = font->GetGlyphInfo(c);

		// 描画位置の計算
		Vector2f position = cursor + glyph.offset;
		Vector2f size     = glyph.size;

		Vector2f rect = (position + size) * GetFontSizeRatio();

		if (rect.x > scale.x) {
			// 範囲外に出る場合は, 改行して描画
			cursor.x = 0;
			cursor.y += font->GetFontSize();

			// 再計算
			position = cursor + glyph.offset;
			rect     = (position + size) * GetFontSizeRatio();
		}

		if (rect.y > scale.y) {
			// 範囲外に出る場合は, 描画終了
			break;
		}

		//!< vertexの設定
		vertices->At(index + 0).position = Vector3f{ position, 0.0f } * GetFontSizeRatio();
		vertices->At(index + 0).texcoord = glyph.uv[0];
		vertices->At(index + 0).color    = color_;

		vertices->At(index + 1).position = Vector3f{ position + Vector2f(size.x, 0.0f), 0.0f } * GetFontSizeRatio();
		vertices->At(index + 1).texcoord = { glyph.uv[1].x, glyph.uv[0].y };
		vertices->At(index + 1).color    = color_;

		vertices->At(index + 2).position = Vector3f{ position + size, 0.0f } * GetFontSizeRatio();
		vertices->At(index + 2).texcoord = glyph.uv[1];
		vertices->At(index + 2).color    = color_;

		vertices->At(index + 3).position = Vector3f{ position + Vector2f(0.0f, size.y), 0.0f } * GetFontSizeRatio();
		vertices->At(index + 3).texcoord = { glyph.uv[0].x, glyph.uv[1].y };
		vertices->At(index + 3).color    = color_;

		index += 4;

		cursor.x += glyph.advance;

	}

}

void TextRendererComponent::BindIABuffer(const DirectXQueueContext* context) {
	input_.BindIABuffer(context);
}

void TextRendererComponent::DrawCall(const DirectXQueueContext* context) {
	context->GetCommandList()->DrawIndexedInstanced(static_cast<UINT>(text_.size() * 6), 1, 0, 0, 0);
}

float TextRendererComponent::GetFontSizeRatio() const {
	return size_ / GetFont()->GetFontSize();
}

const RectTransformComponent* TextRendererComponent::GetRectTransform() const {
	return BaseComponent::GetBehaviour()->GetComponent<RectTransformComponent>();
}

json TextRendererComponent::PerseToJson() const {
	json component = json::object();
	component["text"]  = ToString(text_);
	component["font"]  = font_.Serialize();
	component["color"] = JsonSerializeFormatter<Color4f>::Serialize(color_);
	component["size"]  = size_;

	return component;
}

void TextRendererComponent::InputJson(const json& data) {
	if (!data["text"].is_null()) {
		text_ = ToWString(data["text"].get<std::string>());
	}

	Uuid font = Uuid::Deserialize(data["font"].get<std::string>());

	// fontのuuidが存在しない場合は, tableから読み込み
	if (!sAssetStorage->Contains<AssetFont>(font)) {
		const auto& filepath = sAssetStorage->GetFilepath(font);
		sContentStorage->Import<ContentFont>(filepath);
	}

	font_ = font;

	color_ = JsonSerializeFormatter<Color4f>::Deserialize(data["color"]);
	size_  = JsonSerializeFormatter<float>::Deserialize(data["size"]);
}
