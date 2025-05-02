#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../BaseComponent.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>

//* lib
#include <Lib/Geometry/Vector2.h>
#include <Lib/Geometry/Matrix4x4.h>

//* c++
#include <memory>

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
		GameCamera,
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

	CameraComponent(MonoBehaviour* behaviour) : BaseComponent(behaviour) { Init(); }
	~CameraComponent() override = default;

	json OutputJson() const override;

	void InputJson(const json& data) override;

	void ShowComponentInspector() override;

	void Init();

	//* buffer option *//

	void CreateBuffer();

	const D3D12_GPU_VIRTUAL_ADDRESS& GetGPUVirtualAddress() const;

	//* camera option *//

	void UpdateView();

	void UpdateProj();

	Tag GetTag() const { return tag_; }

	void SetTag(Tag tag) { tag_ = tag; }

	//* getter *//

	const Camera& GetCamera() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Tag tag_ = Tag::None;

	std::unique_ptr<DxObject::DimensionBuffer<Camera>> buffer_;

	Projection projection_ = {};

	//=========================================================================================
	// private methods
	//=========================================================================================

	void PushLineFrustum();

};
