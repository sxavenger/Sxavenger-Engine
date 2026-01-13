#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../BaseComponent.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/DxObject/DxConstantBuffer.h>

//* lib
#include <Lib/Geometry/Vector2.h>
#include <Lib/Geometry/Matrix4x4.h>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// CameraComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class CameraComponent final
	: public BaseComponent {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Tag enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Tag : uint8_t {
		None,
		Game,
		Editor,
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Camera structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Camera {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Init();

		void TransferView(const Matrix4x4& _world);

		void TransferProj(const Matrix4x4& _proj);

		//=========================================================================================
		// public variables
		//=========================================================================================

		Matrix4x4 view;
		Matrix4x4 world;
		Matrix4x4 proj;
		Matrix4x4 projInv;
		float nearZ;
		float farZ;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Projection structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Projection {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Init();

		Matrix4x4 ToProj() const;

		//=========================================================================================
		// public variables
		//=========================================================================================

		Vector2f sensor; /*_mm*/
		float focal;     /*_mm*/
		float nearZ;
		float farZ;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	CameraComponent(EntityBehaviour* behaviour);
	~CameraComponent() override = default;

	void ShowComponentInspector() override;

	//* buffer option *//

	const D3D12_GPU_VIRTUAL_ADDRESS& GetGPUVirtualAddress() const;

	const D3D12_GPU_VIRTUAL_ADDRESS& GetPrevGPUVirtualAddress() const;

	void SwapBuffer();

	//* camera option *//

	void UpdateView();

	void UpdateProj();

	Tag GetTag() const { return tag_; }

	void SetTag(Tag tag) { tag_ = tag; }

	//* getter *//

	const Camera& GetCamera() const;

	const Projection& GetProjection() const { return projection_; }
	Projection& GetProjection() { return projection_; }

	Vector3f GetPosition() const;

	//* helper option *//

	Vector3f CalculateNDCPosition(const Vector3f& point) const;

	Vector3f CalculateWorldPosition(const Vector3f& ndc) const;

	//* json option *//

	json ParseToJson() const override;

	void InputJson(const json& data) override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Tag tag_               = Tag::None;
	Projection projection_ = {};

	std::array<std::unique_ptr<DxObject::ConstantBuffer<Camera>>, 2> buffers_;
	size_t currentIndex_ = 0;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateBuffer();

	void PushLineFrustum();

};

SXAVENGER_ENGINE_NAMESPACE_END
