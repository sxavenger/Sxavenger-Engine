#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// object
#include <RayTracingObject.h>

// DxObject
#include <DxBufferResource.h>
#include <DxStructuredBuffer.h>

// attribute
#include <Attribute.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SubobjectFlag enum
////////////////////////////////////////////////////////////////////////////////////////////
enum SubobjectFlag {
	FLAG_LAMBERT,      // 1 << 0
	FLAG_ALPHARAY,     // 1 << 1
	FLAG_ALPHALAMBERT, // 1 << 2 !< effect用として(LAMBERTと合わせないこと)
	FLAG_PHONG,        // 1 << 3
	FLAG_SHADOW,       // 1 << 4
	FLAG_AO,           // 1 << 5

	kCountOfSubobjectFlag
};

enum AlphaRayType {
	ALPHARAY_PENETRATION,
	ALPHARAY_REFLECTION,

	kCountOfAlphaRayType
};

////////////////////////////////////////////////////////////////////////////////////////////
// Subobject class
////////////////////////////////////////////////////////////////////////////////////////////
class Subobject
	: public RayTracingObject
	, public Attribute {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Subobject() {}

	~Subobject() = default;

	void Init(
		DxObject::BufferResource<VertexData>* vertices, DxObject::IndexBufferResource* indices,
		DxObject::StructuredBuffer* verticesStructuredBuffer, DxObject::StructuredBuffer* indicesStructuredBuffer);

	void SetOnTLAS(DxrObject::TopLevelAS* tlas);

	bool IsDelete() const { return isDelete_; }

	void SetIsDelete(bool isDelete) { isDelete_ = isDelete; }

	void SetAttributeImGui() override;

protected:

	////////////////////////////////////////////////////////////////////////////////////////////
	// SubobjectMaterial structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct SubobjectMaterial {
		Vector4f color       = { 0.0f, 1.0f, 0.0f, 1.0f };
		Vector4f shadowColor = { 0.0f, 0.0f, 0.0f, 0.5f };
		Vector4f aoColor     = {0.0f, 0.0f, 0.0f, 1.0f };
		int flags            = 0;
		int alphaRayType     = ALPHARAY_PENETRATION;
		float aoRange        = 1.0f;
		float aoLimit        = 8 * 8;
	};

	//=========================================================================================
	// protected variables
	//=========================================================================================

	bool isDelete_ = false;

	std::unique_ptr<DxObject::BufferPtrResource<SubobjectMaterial>> materialBuffer_;
	SubobjectMaterial                                               material_;

	//=========================================================================================
	// protected methods
	//=========================================================================================

	void InitMaterial();

	void SetImGuiCommand();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	bool flags_[kCountOfSubobjectFlag] = { false };

};