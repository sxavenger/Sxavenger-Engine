#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// DxObject
#include <DxDevices.h>
#include <DxBufferResource.h>

// c++
#include <memory>

// Geometry
#include <Vector4.h>
#include <Vector3.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DirectionalLight structure
////////////////////////////////////////////////////////////////////////////////////////////
struct DirectionalLight {
	Vector4f color;
	Vector3f direction;
	float intensity;
};

////////////////////////////////////////////////////////////////////////////////////////////
// Directional class
////////////////////////////////////////////////////////////////////////////////////////////
class Directional {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief コンストラクタ
	Directional(DxObject::Devices* devices);

	//! @brief デストラクタ
	~Directional();

	void Init(DxObject::Devices* devices);

	void Term();

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() {
		return resource_->GetGPUVirtualAddress();
	}

	void UpdateImGui(const char* title = "light editor", const char* lightName = "directional light");

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<DxObject::BufferPtrResource<DirectionalLight>> resource_;

	DirectionalLight lightData_;

	// TODO: Lightクラスからの継承等々...

};