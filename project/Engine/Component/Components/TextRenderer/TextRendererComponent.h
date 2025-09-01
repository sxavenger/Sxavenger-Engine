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
#include <Engine/Preview/Asset/UAssetFont.h>
#include <Engine/Preview/Asset/UAssetParameter.h>

////////////////////////////////////////////////////////////////////////////////////////////
// TextRendererComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class TextRendererComponent final
	: public BaseComponent {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	TextRendererComponent(MonoBehaviour* behaviour) : BaseComponent(behaviour) { CreateBuffer(); }
	~TextRendererComponent() override = default;

	void CreateBuffer();

	void ShowComponentInspector() override;

	//* component option *//

	void SetText(const std::wstring& text);

	void SetFont(const UAssetParameter<UAssetFont>& font) { font_ = font; }

	//* buffer option *//

	void PerseText();

	void BindIABuffer(const DirectXQueueContext* context);

	void DrawCall(const DirectXQueueContext* context);

	//* getter *//

	bool IsEnable() const { return !text_.empty() && !font_.Empty(); }

	const std::shared_ptr<UAssetFont> GetFont() const { return font_.WaitRequire(); }

	//* behaviour option *//

	const RectTransformComponent* GetRectTransform() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* text *//

	std::wstring text_;
	InputUIVertex input_;

	UAssetParameter<UAssetFont> font_;

	//* parameter *//

	static const inline size_t kMaxTextLength = 256;

};
