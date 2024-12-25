#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include <Engine/Module/Behavior/TransformBehavior.h>

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
	: public TransformBehavior {
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

	void DrawFrustum(const Color4f& color, const std::optional<float>& length);

	virtual json OutputJson() override;

	virtual void InputJson(const json& data) override;

	void SystemAttributeImGui() override;

	//* getter *//

	const D3D12_GPU_VIRTUAL_ADDRESS& GetGPUVirtualAddress() const;

	const Matrix4x4& GetView() const { return (*buffer_)[0].viewMatrix; }

	const Matrix4x4& GetProj() const { return (*buffer_)[0].projMatrix; }

	const Matrix4x4& GetProjInverse() const { return (*buffer_)[0].projInverseMatrix; }

	Matrix4x4 GetViewProj() const { return GetView() * GetProj(); }

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
		float nearZ;
		float farZ;
	};

	//=========================================================================================
	// protected variables
	//=========================================================================================

	std::unique_ptr<DxObject::DimensionBuffer<Camera3dForGPU>> buffer_;

};
