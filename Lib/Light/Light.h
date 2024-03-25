#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// c++
#include <memory>
#include <numbers>

// DirectX12
#include <DxDevices.h>
#include <DxBufferResource.h>

// Geometry
#include <Vector3.h>
#include <Vector4.h>
#include <Matrix4x4.h>

#include <ObjectStructure.h>

////////////////////////////////////////////////////////////////////////////////////////////
// LightType enum
////////////////////////////////////////////////////////////////////////////////////////////
enum LightType {
	TYPE_DIRECTIONAL,
	TYPE_POINT,
	TYPE_SPOT,

	kLightTypeCount
};

////////////////////////////////////////////////////////////////////////////////////////////
// Light class
////////////////////////////////////////////////////////////////////////////////////////////
class Light {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Light(DxObject::Devices* device) { Init(device); }

	~Light() { Term(); }

	void Init(DxObject::Devices* device);

	void UpdateImGui(const char* windowName = "light editor", const char* treeName = "light");

	void Term();

	const D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() {
		return resource_->GetGPUVirtualAddress();
	}

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// LightData structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct LightData {
		Vector4f color;
		Vector4f position;
		Vector3f direction;
		float    intensity;
		float    range;
		float    decay;
		float    angle;
		float    falloffAngle;

		int      lightType;

		LightData() {
			color        = { 1.0f, 1.0f, 1.0f, 1.0f };
			position     = { 0.0f, 0.0f, 0.0f, 1.0f };
			direction    = { 0.0f, 0.0f, 1.0f };
			intensity    = 1.0f;
			range        = 0.0f;
			decay        = 1.0f;
			angle        = std::cos(std::numbers::pi_v<float> / 3.0f);
			falloffAngle = std::cos(std::numbers::pi_v<float> / 4.0f);

			lightType = TYPE_DIRECTIONAL;
		}
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	static const char* item[kLightTypeCount];

	// resource
	std::unique_ptr<DxObject::BufferPtrResource<LightData>> resource_;
	LightData                                               lightData_;

	// parameter
	Vector3f rotate_ = { 0.0f, 0.0f, 0.0f };

};