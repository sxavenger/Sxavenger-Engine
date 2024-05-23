#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// Geometry
#include <Matrix4x4.h>
#include <Vector4.h>

// c++
#include <string>
#include <memory>
#include <numbers>

// DxObject
#include <DxBufferResource.h>

#include <Attribute.h>

////////////////////////////////////////////////////////////////////////////////////////////
// RayTracingCamera class
////////////////////////////////////////////////////////////////////////////////////////////
class RayTracingCamera
	: public Attribute {
private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Camera structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Camera { //!< Transform
		Vector3f scale;
		Vector3f rotate;
		Vector3f translate;
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	RayTracingCamera() { Init(); }

	~RayTracingCamera() { Term(); }

	void Init();

	void Term();

	void SetAttributeImGui() override;

	const D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() {
		return constantBuffer_->GetGPUVirtualAddress();
	}

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// CameraForGPU structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct CameraForGPU {
		Matrix4x4 world; //!< viewInverse
		Matrix4x4 projInverse;
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	Camera transform_;

	Matrix4x4 projectionMatrix_;

	// ConstantBuffer //
	std::unique_ptr<DxObject::BufferPtrResource<CameraForGPU>> constantBuffer_;
	CameraForGPU cameraForGpu_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void SetTransform(const Vector3f& scale, const Vector3f& rotate, const Vector3f& translate);

	void SetProjection(float fovY, float aspectRatio, float nearClip, float farClip);

	void RecalculateCamera();

};