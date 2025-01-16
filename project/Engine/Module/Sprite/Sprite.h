#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include <Engine/Module/Component/Transform2dComponent.h>

//* engine
#include <Engine/Content/InputAssembler/InputAssembler.h>

//* c++
#include <memory>
#include <optional>
#include <bitset>

////////////////////////////////////////////////////////////////////////////////////////
// Sprite class
////////////////////////////////////////////////////////////////////////////////////////
class Sprite
	: protected Transform2dComponent {
public:

	////////////////////////////////////////////////////////////////////////////////////////
	// VertexId enum class
	////////////////////////////////////////////////////////////////////////////////////////
	enum class VertexId : uint8_t {
		LeftTop,
		RightTop,
		LeftBottom,
		RightBottom,
	};
	
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Sprite()  = default;
	~Sprite() = default;

	void Init();

	//* option *//

	void SetPosition(const Vector2f& position);

	void SetSize(const Vector2f& size);

	void SetColor(const Color4f& color, VertexId id);
	void SetColor(const Color4f& color);

	//* texture *//

	void SetHandle(const D3D12_GPU_DESCRIPTOR_HANDLE& handle) { handle_ = handle; }

	//* render *//

	void Draw();

private:

	////////////////////////////////////////////////////////////////////////////////////////
	// SpriteVertex structure
	////////////////////////////////////////////////////////////////////////////////////////
	struct SpriteVertex {
		Vector4f position;
		Vector2f texcoord;
		Color4f  color;
	};

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* vertex *//

	InputAssembler<SpriteVertex> ia_;

	//* parameter *// 

	std::optional<D3D12_GPU_DESCRIPTOR_HANDLE> handle_ = std::nullopt;

	Vector2f pivot_  = kOrigin2<float>;
	Vector2f anchor_ = kOrigin2<float>;

	std::bitset<2> isFlip_ = {};

	//=========================================================================================
	// private methods
	//=========================================================================================

	void UpdateVertex();
	void UpdateTexcoord();

};
