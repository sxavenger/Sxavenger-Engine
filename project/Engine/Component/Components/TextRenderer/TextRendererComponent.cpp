#include "TextRendererComponent.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../Entity/MonoBehaviour.h"

////////////////////////////////////////////////////////////////////////////////////////////
// TextRendererComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void TextRendererComponent::CreateBuffer() {
	input_.Create(static_cast<uint32_t>(kMaxTextLength * 4), static_cast<uint32_t>(kMaxTextLength * 2));

	auto vertices = input_.GetVertex();
	auto indices  = input_.GetIndex();

	// vertexの初期化
	vertices->Fill(UIVertexData{});

	// indexの初期化
	for (size_t i = 0; i < kMaxTextLength; ++i) {
		indices->At(i * 2 + 0) = { static_cast<UINT>(i * 4 + 0), static_cast<UINT>(i * 4 + 1), static_cast<UINT>(i * 4 + 2) };
		indices->At(i * 2 + 1) = { static_cast<UINT>(i * 4 + 0), static_cast<UINT>(i * 4 + 2), static_cast<UINT>(i * 4 + 3) };
	}
}

void TextRendererComponent::ShowComponentInspector() {

}

void TextRendererComponent::SetText(const std::wstring& text) {
	text_ = text.substr(0, kMaxTextLength - 1);
}

void TextRendererComponent::PerseText() {
	if (text_.empty()) {
		return;
	}

	if (font_.Empty()) {
		Logger::WarningRuntime("[TextRendererComponent] warning | font is not set.");
		return;
	}

	// fontの取得
	const std::shared_ptr<UAssetFont> font = font_.WaitRequire();

	Vector2f cursor = {};
	size_t index    = 0;

	auto vertices = input_.GetVertex();

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

		const UAssetFont::GlyphInfo& glyph = font->GetGlyphInfo(c);

		// 描画位置の計算
		Vector2f position = cursor + glyph.offset;
		Vector2f size     = glyph.size;

		// todo: pivot, anchorの設定

		//!< vertexの設定
		vertices->At(index + 0).position = { position, 0.0f };
		vertices->At(index + 0).texcoord = glyph.uv[0];
		vertices->At(index + 0).color    = { 1.0f, 1.0f, 1.0f, 1.0f };

		vertices->At(index + 1).position = { position + Vector2f(size.x, 0.0f), 0.0f };
		vertices->At(index + 1).texcoord = { glyph.uv[1].x, glyph.uv[0].y };
		vertices->At(index + 1).color    = { 1.0f, 1.0f, 1.0f, 1.0f };

		vertices->At(index + 2).position = { position + size, 0.0f };
		vertices->At(index + 2).texcoord = glyph.uv[1];
		vertices->At(index + 2).color    = { 1.0f, 1.0f, 1.0f, 1.0f };

		vertices->At(index + 3).position = { position + Vector2f(0.0f, size.y), 0.0f };
		vertices->At(index + 3).texcoord = { glyph.uv[0].x, glyph.uv[1].y };
		vertices->At(index + 3).color    = { 1.0f, 1.0f, 1.0f, 1.0f };

		index += 4;

		// todo: priorityの設定
		// todo: colorの設定

		cursor.x += glyph.advance;

		// todo: 範囲を作成

	}

}

void TextRendererComponent::BindIABuffer(const DirectXQueueContext* context) {
	input_.BindIABuffer(context);
}

void TextRendererComponent::DrawCall(const DirectXQueueContext* context) {
	context->GetCommandList()->DrawIndexedInstanced(static_cast<UINT>(text_.size() * 6), 1, 0, 0, 0);
}

const RectTransformComponent* TextRendererComponent::GetRectTransform() const {
	return BaseComponent::GetBehaviour()->GetComponent<RectTransformComponent>();
}
