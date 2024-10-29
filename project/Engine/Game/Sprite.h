#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DxObject/DxGraphicsPipeline.h>
#include <Engine/System/DxObject/DxBufferResource.h>
#include <Engine/System/Texture.h>

//* lib
#include <Lib/Geometry/Vector4.h>
#include <Lib/Geometry/Matrix4x4.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SpriteCommon class
////////////////////////////////////////////////////////////////////////////////////////////
class SpriteCommon {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	SpriteCommon()  = default;
	~SpriteCommon() { Term(); }

	void Init();

	void Term();

	void Reset();

	//* sprite option *//

	void DrawSprite(
		const Vector2f& pos, const Vector2f& size, const D3D12_GPU_DESCRIPTOR_HANDLE& handle, const Color4f& color = { 1.0f, 1.0f, 1.0f, 1.0f }
	);

	void DrawSprite(
		const Vector2f& pos, const Vector2f& size, float rotate, const D3D12_GPU_DESCRIPTOR_HANDLE& handle, const Color4f& color = { 1.0f, 1.0f, 1.0f, 1.0f }
	);

	void DrawSpriteClip(
		const Vector2f& pos, const Vector2f& size, const Vector2f& leftTopUV, const Vector2f& sizeUV, const D3D12_GPU_DESCRIPTOR_HANDLE& handle, const Color4f& color = { 1.0f, 1.0f, 1.0f, 1.0f }
	);

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// SpriteIAType enum
	////////////////////////////////////////////////////////////////////////////////////////////
	enum SpriteIAType : uint32_t {
		LEFTTOP,
		LEFTBOTTOM,
		RIGHTTOP,
		RIGHTBOTTOM,
		kCountOfSpriteIAType
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// SpriteInputAssembler structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct SpriteInputAssembler {
		Vector4f position;
		Vector2f texcoord;
		Color4f  color;
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// SpriteInfo structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct SpriteInfo {
		Matrix4x4 transform2d;
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// SpriteElement structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct SpriteElement {
		std::unique_ptr<DxObject::BufferResource<SpriteInputAssembler>> vertex;
		std::unique_ptr<DxObject::BufferResource<SpriteInfo>>           info;
		D3D12_GPU_DESCRIPTOR_HANDLE                                     texture;
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* parameter *//

	static const uint32_t kMaxSpriteCount_ = 128;
	uint32_t elementIndex_;

	//* buffer *//

	std::array<SpriteElement, kMaxSpriteCount_> elements_;
	std::unique_ptr<DxObject::IndexBufferResource> index_; //!< indexBufferは共通

	std::unique_ptr<DxObject::BufferResource<Matrix4x4>> projection_;

	//* pipeline *//

	std::unique_ptr<DxObject::GraphicsPipeline> pipeline_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreatePipeline();
	void CreateBuffer();

	void SetVertex(uint32_t index, const Vector2f& anchor = { 0.0f, 0.0f }, const Vector2<bool>& isFlip = {});
	void SetTexcoord(uint32_t index, const Vector2f& pivot = { 0.0f, 0.0f });
	void SetColor(uint32_t index, const Color4f& color);
	void SetTransform2d(uint32_t index, const Vector2f& size, float rotate, const Vector2f& pos);
	void SetHandle(uint32_t index, const D3D12_GPU_DESCRIPTOR_HANDLE& handle);

	void SetTexcoordClip(int32_t index, const Vector2f& leftTopUV, const Vector2f& sizeUV, const Vector2f& pivot = { 0.0f, 0.0f });

	void DrawCall(uint32_t index);

};