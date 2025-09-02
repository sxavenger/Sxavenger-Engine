#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../BaseComponent.h"
#include "../Transform/RectTransformComponent.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>
#include <Engine/Content/InputGeometry/InputUIVertex.h>
#include <Engine/Preview/Asset/UAssetTexture.h>
#include <Engine/Preview/Asset/UAssetParameter.h>

//* lib
#include <Lib/Geometry/Color4.h>
#include <Lib/Transform/Transform.h>

//* external
#include <magic_enum.hpp>

//* c++
#include <bitset>

////////////////////////////////////////////////////////////////////////////////////////////
// SpriteRendererComponent class
////////////////////////////////////////////////////////////////////////////////////////////
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

	SpriteRendererComponent(MonoBehaviour* behaviour) : BaseComponent(behaviour) { CreateBuffer(); }
	~SpriteRendererComponent() override = default;

	void CreateBuffer();

	virtual void ShowComponentInspector() override;

	//* component option *//

	void SetVertexColor(const Color4f& color, VertexPoint point);
	void SetColor(const Color4f& color);

	const Transform2d& GetTransformUV() const { return transformUV_; }
	Transform2d& GetTransformUV() { return transformUV_; }

	bool IsEnable() const { return isEnable_; }

	//* render option *//

	void BindAIBuffer(const DirectXQueueContext* context);

	void DrawCall(const DirectXQueueContext* context);

	const UAssetParameter<UAssetTexture>& GetTextureParameter() const { return texture_; }

	const D3D12_GPU_VIRTUAL_ADDRESS& GetGPUVirtualAddressUV() const { return bufferUV_->GetGPUVirtualAddress(); }

	//* behaviour option *//

	const RectTransformComponent* GetRectTransform() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	bool isEnable_ = true;

	//* input assembler *//

	InputUIVertex input_;

	//* transform buffer *//

	Transform2d transformUV_;
	std::unique_ptr<DxObject::DimensionBuffer<Matrix4x4>> bufferUV_;

	// todo: unityを参考にして使いやすくする.
	// filedとか

	std::bitset<2> isFlip2d_ = {}; //!< x, y
	std::bitset<2> isFlipUV_ = {}; //!< x, y

	UAssetParameter<UAssetTexture> texture_ = nullptr;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void TransferPosition();
	void TransferTexcoord();

	//* imgui helper methods *//

	void ShowTexture();

};
