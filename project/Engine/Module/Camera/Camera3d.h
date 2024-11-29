#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include <Engine/Module/Transform/TransformComponent.h>

//* engine
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>

//* lib
#include <Lib/Geometry/Vector4.h>
#include <Lib/Geometry/Matrix4x4.h>

//* c++
#include <memory>
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////
// Camera3d class
////////////////////////////////////////////////////////////////////////////////////////////
class Camera3d
	: public TransformComponent {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Camera3d()  = default;
	~Camera3d() { Term(); }

	void Create();

	void Term();

	void Reset();

	void UpdateMatrix();

	void SetProjection(float fovY, float aspectRatio, float nearClip, float farClip);

	void DrawFrustum(const Color4f& color, const std::optional<float>& lenght); // TODO:

	virtual json OutputJson() override;

	virtual void InputJson(const json& data) override;

	const D3D12_GPU_VIRTUAL_ADDRESS& GetGPUVirtualAddress() const;

protected:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Camera3dForGPU structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Camera3dForGPU {
		Matrix4x4 viewMatrix        = Matrix4x4::Identity();
		Matrix4x4 worldMatrix       = Matrix4x4::Identity();
		Matrix4x4 projMatrix        = Matrix4x4::Identity();
		Matrix4x4 projInverseMatrix = Matrix4x4::Identity();
		Vector3f position;
	};

	//=========================================================================================
	// protected variables
	//=========================================================================================

	std::unique_ptr<DxObject::DimensionBuffer<Camera3dForGPU>> buffer_;

};