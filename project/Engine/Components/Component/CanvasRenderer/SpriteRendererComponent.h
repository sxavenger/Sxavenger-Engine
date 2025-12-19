#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../BaseComponent.h"
#include "../Transform/RectTransformComponent.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/DxObject/DxConstantBuffer.h>
#include <Engine/Graphics/InputGeometry/InputCanvasVertex.h>
#include <Engine/Assets/Asset/AssetTexture.h>
#include <Engine/Assets/Asset/AssetParameter.h>

//* lib
#include <Lib/Geometry/Color4.h>
#include <Lib/Transform/Transform.h>

//* external
#include <magic_enum.hpp>

//* c++
#include <bitset>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// SpriteRendererComponent class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief 2DSprite描画component.
class SpriteRendererComponent final
	: public BaseComponent {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// VertexPoint enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class VertexPoint : uint8_t {
		LeftTop     = 0,
		RightTop    = 1,
		LeftBottom  = 2,
		RightBottom = 3,
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	SpriteRendererComponent(MonoBehaviour* behaviour);
	~SpriteRendererComponent() override = default;

	void ShowComponentInspector() override;

	//* component option *//

	void SetVertexColor(const Color4f& color, VertexPoint point);
	void SetColor(const Color4f& color);

	const Transform2d& GetTransformUV() const { return transformUV_; }
	Transform2d& GetTransformUV() { return transformUV_; }

	bool IsEnable() const { return isEnable_; }

	//* render option *//

	void BindAIBuffer(const DirectXQueueContext* context);

	void DrawCall(const DirectXQueueContext* context);

	const AssetParameter<AssetTexture>& GetTextureParameter() const { return texture_; }

	const D3D12_GPU_VIRTUAL_ADDRESS& GetGPUVirtualAddressUV() const { return bufferUV_->GetGPUVirtualAddress(); }

	//* behaviour option *//

	const RectTransformComponent* GetRectTransform() const;

	//* json option *//

	json PerseToJson() const override;

	void InputJson(const json& data) override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	bool isEnable_ = true;

	//* input assembler *//

	InputCanvasVertex input_;

	std::array<Color4f, magic_enum::enum_count<VertexPoint>()> color_ = {};

	//* transform buffer *//

	Transform2d transformUV_;
	std::unique_ptr<DxObject::ConstantBuffer<Matrix4x4>> bufferUV_;

	// todo: unityを参考にして使いやすくする.
	// filedとか

	AssetParameter<AssetTexture> texture_ = nullptr;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void TransferPosition();
	void TransferTexcoord();
	void TransferColor();

	//* imgui helper methods *//

	void ShowTexture();

};

SXAVENGER_ENGINE_NAMESPACE_END
