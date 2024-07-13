#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// DxObject
#include <DxBufferResource.h>

// Geometry
#include <Vector4.h>
#include <Vector3.h>

// lib
#include <MathLib.h>

// c++
#include <memory>

// attribute
#include <Attribute.h>

#include <Light.h>

////////////////////////////////////////////////////////////////////////////////////////////
// RayTracingLight class
////////////////////////////////////////////////////////////////////////////////////////////
class RayTracingLight
	: public Attribute {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	RayTracingLight() { Init(); }

	~RayTracingLight() { Term(); }

	void Init();

	void Term();

	void SetAttributeImGui() override;

	const D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const {
		return resource_->GetGPUVirtualAddress();
	}

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// DirectionalLight structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct DirectionalLight {
		Color4f color     = { 1.0f, 1.0f, 1.0f, 1.0f };
		Vector4f position  = { 0.0f, 0.0f, 0.0f, 1.0f };
		Vector3f direction = { 0.0f, -1.0f, 0.0f };
		float intensity    = 1.0f;
		float range        = 0.0f;
		float decay        = 1.0f;
		float angle        = std::cos(pi_v / 3.0f);

		int type = LIGHT_DIRECTIONAL;
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<DxObject::BufferPtrResource<DirectionalLight>> resource_;
	DirectionalLight data_;

};