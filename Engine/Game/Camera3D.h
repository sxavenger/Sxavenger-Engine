#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// geometry
#include <Lib/Geometry/Vector4.h>
#include <Lib/Geometry/Matrix4x4.h>

// c++
#include <memory>
#include <numbers>

// DxObject
#include <Engine/System/DxObject/DxBufferResource.h>

// engine
#include "Transform.h"

// attribute
#include <Engine/System/Attribute.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Camera3D class
////////////////////////////////////////////////////////////////////////////////////////////
class Camera3D {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief コンストラクタ
	Camera3D() { Init(); }

	//! @brief デストラクタ
	~Camera3D() { Term(); }

	void Init();

	void Term();

	//* camera setter *//

	void SetTransform(const Vector3f& scale, const Vector3f& rotate, const Vector3f& translate);

	void SetProjection(float fovY, float aspectRatio, float nearClip, float farClip);

	//* camera getter *//

	const Matrix4x4 GetViewProjMatrix() const { return (*buffer_)[0].viewMatrix * (*buffer_)[0].projMatrix; }

	//* buffer address *//

	const D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const { return buffer_->GetGPUVirtualAddress(); }

	//* debug drawer *//

	void DrawFrustum(const Color4f& color) const;

protected:

	////////////////////////////////////////////////////////////////////////////////////////////
	// CameraForGPU structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct CameraForGPU {
		Vector4f position;
		Matrix4x4 viewMatrix = Matrix4x4::Identity();
		Matrix4x4 projMatrix = Matrix4x4::Identity();
	};

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* camera transform *//

	EulerTransform transform_;

	//* buffer *//

	std::unique_ptr<DxObject::BufferResource<CameraForGPU>> buffer_;

	//=========================================================================================
	// protected methods
	//=========================================================================================

	void CalculateView();

};
