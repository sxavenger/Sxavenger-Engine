#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../BaseComponent.h"
#include "../Transform/RectTransformComponent.h"

//* engine
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>
#include <Engine/Content/InputGeometry/InputUIVertex.h>
#include <Engine/Preview/Asset/AssetFont.h>
#include <Engine/Preview/Asset/AssetParameter.h>

////////////////////////////////////////////////////////////////////////////////////////////
// TextRendererComponent class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief テキスト描画component.
class TextRendererComponent final
	: public BaseComponent {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	TextRendererComponent(MonoBehaviour* behaviour);
	~TextRendererComponent() override = default;

	void ShowComponentInspector() override;

	//* component option *//

	void SetText(const std::wstring& text);

	void SetFont(const AssetParameter<AssetFont>& font) { font_ = font; }

	void SetSize(float size) { size_ = size; }

	void SetColor(const Color4f& color) { color_ = color; }

	//* render option *//

	void PerseText();

	void BindIABuffer(const DirectXQueueContext* context);

	void DrawCall(const DirectXQueueContext* context);

	//* getter *//

	bool IsEnable() const { return !text_.empty() && !font_.Empty(); }

	const std::shared_ptr<AssetFont> GetFont() const { return font_.WaitRequire(); }

	//! @brief AssetのFontSizeとParameterのFonstSize比率を取得
	float GetFontSizeRatio() const;

	//* behaviour option *//

	const RectTransformComponent* GetRectTransform() const;

	//* json option *//

	json PerseToJson() const override;

	void InputJson(const json& data) override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================


	std::wstring text_;

	AssetParameter<AssetFont> font_;

	Color4f color_ = kWhite4<float>;

	float size_ = 12.0f;

	//* parameter *//

	static const inline size_t kMaxTextLength = 256;

	InputUIVertex input_;

};
