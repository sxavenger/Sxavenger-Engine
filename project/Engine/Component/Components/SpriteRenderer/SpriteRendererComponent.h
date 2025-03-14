#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../BaseComponent.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>
#include <Engine/Content/InputAssembler/TriangleInputAssembler.h>
#include <Engine/Asset/Observer/AssetObserver.h>
#include <Engine/Asset/Texture/Texture.h>

//* lib
#include <Lib/Geometry/Color.h>
#include <Lib/Transform/Transform.h>

//* c++
#include <bitset>
#include <variant>
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// SpriteRendererComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class SpriteRendererComponent
	: public BaseComponent {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////
	using Texture = std::variant<D3D12_GPU_DESCRIPTOR_HANDLE, AssetObserver<AssetTexture>>;

	////////////////////////////////////////////////////////////////////////////////////////////
	// Transformation2d structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Transformation2d {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Init();

		void Transfer(const Matrix4x4& _mat);

		//=========================================================================================
		// public variables
		//=========================================================================================

		Matrix4x4 mat;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// VertexPoint enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class VertexPoint : uint8_t {
		LeftTop,     //!< 左上
		RightTop,    //!< 右上
		LeftBottom,  //!< 左下
		RightBottom, //!< 右下
	};
	static const uint8_t kVertexPointCount = 4;

	////////////////////////////////////////////////////////////////////////////////////////////
	// Vertex structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Vertex {
		Vector3f position; //* screen position and (layer depth)
		Vector2f texcoord;
		Color4f  color;
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// GetDescriptorVisitor structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct GetDescriptorVisitor {
	public:

		const D3D12_GPU_DESCRIPTOR_HANDLE& operator()(const D3D12_GPU_DESCRIPTOR_HANDLE& handle);
		const D3D12_GPU_DESCRIPTOR_HANDLE& operator()(const AssetObserver<AssetTexture>& observer);

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	SpriteRendererComponent(MonoBehaviour* behaviour) : BaseComponent(behaviour) { Create(); }
	~SpriteRendererComponent() override = default;

	void Create();

	virtual void InspectorImGui() override;

	json OutputJson() const override;

	void InputJson(const json& data) override;

	//* sprite option *//

	void SetTexture(const D3D12_GPU_DESCRIPTOR_HANDLE& handle) { texture_ = handle; }
	void SetTexture(const AssetObserver<AssetTexture>& observer) { texture_ = observer; }

	void SetPosition(const Vector2f& position);
	void SetRotate(float rotate);
	void SetSize(const Vector2f& size);

	void SetVertexColor(const Color4f& color, VertexPoint point);
	void SetColor(const Color4f& color);

	void SetAnchor(const Vector2f& anchor) { anchor_ = anchor; }
	void SetPivot(const Vector2f& pivot) { pivot_ = pivot; }
	void SetPriority(uint16_t priority) { priority_ = priority; }

	//* render option *//

	bool CheckTexture() const { return texture_.has_value(); }

	void BindAIBuffer(const DirectXThreadContext* context);

	void DrawCall(const DirectXThreadContext* context);

	//* getter *//

	const D3D12_GPU_VIRTUAL_ADDRESS& GetTransform2dBuffer() const { return buffer_->GetGPUVirtualAddress(); }

	const D3D12_GPU_VIRTUAL_ADDRESS& GetTransformUVBuffer() const { return bufferUV_->GetGPUVirtualAddress(); }

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetTexture() const;

	const Transform2d& GetTransform2d() const { return transform2d_; }
	Transform2d& GetTransform2d() { return transform2d_; }

	const Transform2d& GetTransformUV() const { return transformUV_; }
	Transform2d& GetTransformUV() { return transformUV_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* transform sprite *//

	Transform2d transform2d_;
	std::unique_ptr<DxObject::DimensionBuffer<Transformation2d>> buffer_;

	//* transform uv *//

	Transform2d transformUV_;
	std::unique_ptr<DxObject::DimensionBuffer<Transformation2d>> bufferUV_;

	//* texture *//

	std::optional<Texture> texture_;

	//* parameter *//

	Vector2f anchor_ = kOrigin2<float>;
	Vector2f pivot_  = kOrigin2<float>;

	std::bitset<2> isFlip2d_ = {};
	std::bitset<2> isFlipUV_ = {};

	uint16_t priority_ = 0;

	//* triangle input assembler *//

	TriangleInputAssembler<Vertex> ia_;

	std::array<Color4f, kVertexPointCount> colors_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateInputAssembler();

	void TransferPosition();
	void TransferColor();
	void TransferTexcoord();
	void TransferMatrix();

};
