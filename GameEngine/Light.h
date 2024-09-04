#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* _DXOBJECT
#include <DxBufferResource.h>

//* Geometry
#include <Vector4.h>
#include <Vector3.h>

//* attribute
#include <Attribute.h>

////////////////////////////////////////////////////////////////////////////////////////////
// LightType enum
////////////////////////////////////////////////////////////////////////////////////////////
enum LightType : int32_t {
	LIGHT_NONE,
	LIGHT_DIRECTIONAL,
	LIGHT_POINT,
	LIGHT_SPOT,

	kCountOfLightType
};

////////////////////////////////////////////////////////////////////////////////////////////
// Light structure
////////////////////////////////////////////////////////////////////////////////////////////

struct Light {
	Color4f  color; //!< rgb: color, a: intensity
	Vector4f position;
	Vector3f direction;
	float    range;
	float    decay;
	float    angle;
	float    falloffAngle;

	LightType type;

	void SetImGuiCommand();
};

struct DirectionalLight {
	Color4f  color;
	Vector4f position;
	Vector3f direction;
};

struct PointLight {
	Color4f  color;
	Vector4f position;
	float    range;
	float    decay;
};

struct SpotLight {
	Color4f  color;
	Vector4f position;
	Vector3f direction;
	float    range;
	float    decay;
	float    angle;
	float    falloffAngle;
};

////////////////////////////////////////////////////////////////////////////////////////////
// LightBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
class LightBuffer
	: public Attribute {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	LightBuffer() = default;

	void Init();

	void Term();

	//* light option *//

	void SetDirectionalLight(uint32_t index, const DirectionalLight& directional);

	void SetPointLight(uint32_t index, const PointLight& point);

	void SetSpotLight(uint32_t index, const SpotLight& spot);

	//* buffer *//

	const D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const { return buffer_->GetGPUVirtualAddress(); }

	//* Attribute *//

	void SetAttributeImGui() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* parameter *//

	static const uint32_t kMaxLightNum_ = 8;
	
	//* buffer *//

	std::unique_ptr<DxObject::BufferResource<Light>> buffer_;

};