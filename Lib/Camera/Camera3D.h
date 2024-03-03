#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// Geometry
#include <Matrix4x4.h>
#include <Vector4.h>

// c++
#include <string>

// DxObject
#include <DxBufferResource.h>

#include <ObjectPtr.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Camera3D class
////////////////////////////////////////////////////////////////////////////////////////////
class Camera3D {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief コンストラクタ
	Camera3D();

	Camera3D(const std::string& filePath);

	//! @brief デストラクタ
	~Camera3D();

	//! @brief 初期化処理
	void Init();

	void Term();

	//! @brief カメラ情報を設定
	//! 
	//! @param[in] scale
	//! @param[in] rotate
	//! @param[in] translate
	void SetCamera(const Vector3f& scale, const Vector3f& rotate, const Vector3f& transform);

	void SetProjection(float fovY, float aspectRatio, float nearClip, float farClip);

	void UpdateImGui(const char* title = "camera editor", const char* cameraName = "camera");

	void ReadJsonCameraData(const std::string& filePath);

	const Matrix4x4 GetViewProjectionMatrix() const { return viewMatrix_ * projectionMatrix_; }

	const D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() {
		return resource_->GetGPUVirtualAddress();
	}

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Camera structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Camera { //!< Transform
		Vector3f scale;
		Vector3f rotate;
		Vector3f translate;
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	Camera camera_;

	Matrix4x4 viewMatrix_;
	Matrix4x4 projectionMatrix_;

	Object_Ptr<DxObject::BufferPtrResource<Vector4f>> resource_;
	Vector4f position;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void RecalculateCamera();
};