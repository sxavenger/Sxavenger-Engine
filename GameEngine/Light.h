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