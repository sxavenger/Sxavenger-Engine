#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* layer
#include "../LLayer.h"

//* engine
#include <Engine/Content/InputAssembler/TriangleInputAssembler.h>
#include <Engine/Content/Component/Transform2dComponent.h>
#include <Engine/Content/Component/TextureComponent.h>
#include <Engine/Asset/Texture/AssetTexture.h>
#include <Engine/Asset/Observer/AssetObserver.h>

//* lib
#include <Lib/Geometry/Vector2.h>

//* c++
#include <bitset>

////////////////////////////////////////////////////////////////////////////////////////////
// LSprite class
////////////////////////////////////////////////////////////////////////////////////////////
class LSprite
	: public LLayer, protected Transform2dComponent, public TextureComponent {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////
	using TextureType = std::variant<D3D12_GPU_DESCRIPTOR_HANDLE, AssetObserver<AssetTexture>>;

	////////////////////////////////////////////////////////////////////////////////////////////
	// VertexPoint enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class VertexPoint : uint8_t {
		LeftTop,     //!< 左上
		RightTop,    //!< 右上
		LeftBottom,  //!< 左下
		RightBottom, //!< 右下
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Vertex structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Vertex {
		Vector3f position; //* screen position and layer depth
		Vector2f texcoord;
		Color4f  color;
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	LSprite()          = default;
	virtual ~LSprite() = default;

	void Init();

	//* render *//

	virtual void Render(const RenderContext& context) override;

	//* option *//

	void TransferColor(const Color4f& color,VertexPoint point);
	void TransferColor(const Color4f& color);

	//* setter *//

	void SetTexture(const D3D12_GPU_DESCRIPTOR_HANDLE& handle) { texture_   = handle; }
	void SetTexture(const AssetObserver<AssetTexture>& observer) { texture_ = observer; }

	void SetPosition(const Vector2f& position);

	void SetSize(const Vector2f& size);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	TriangleInputAssembler<Vertex> ia_;
	std::optional<TextureType>     texture_;

	Vector2f anchor_ = kOrigin2<float>;
	Vector2f pivot_  = kOrigin2<float>;

	std::bitset<2> isFlip_ = {};

	float depth_ = 0.0f;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void UpdatePosition();
	void UpdateTexcoord();

};
