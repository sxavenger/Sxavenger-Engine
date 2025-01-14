#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include <Engine/Module/Component/Transform2dComponent.h>

//* engine
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>

//* c++
#include <memory>
#include <optional>
#include <bitset>

////////////////////////////////////////////////////////////////////////////////////////
// Sprite class
////////////////////////////////////////////////////////////////////////////////////////
class Sprite
	: public Transform2dComponent {
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

	//* texture *//

	void SetHandle(D3D12_GPU_DESCRIPTOR_HANDLE handle) { handle_ = handle; }

private:

	////////////////////////////////////////////////////////////////////////////////////////
	// SpriteVertex structure
	////////////////////////////////////////////////////////////////////////////////////////
	struct SpriteVertex {
		Vector3f position;
		Vector2f uv;
		Color4f  color;
	};

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* vertex *//

	std::unique_ptr<DxObject::VertexDimensionBuffer<SpriteVertex>> vertex_;

	//* parameter *// 

	std::optional<D3D12_GPU_DESCRIPTOR_HANDLE> handle_ = std::nullopt;

	Vector2f pivot_  = kOrigin2<float>;
	Vector2f anchor_ = kOrigin2<float>;

	std::bitset<2> isFlip_ = 0;

};
