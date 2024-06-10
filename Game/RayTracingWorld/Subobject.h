#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// object
#include <RayTracingObject.h>

// DxObject
#include <DxBufferResource.h>
#include <DxStructuredBuffer.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Subobject base class
////////////////////////////////////////////////////////////////////////////////////////////
class Subobject
	: public RayTracingObject {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Subobject() {}

	virtual ~Subobject() = default;

	virtual void Init(
		DxObject::BufferResource<VertexData>* vertices, DxObject::IndexBufferResource* indices,
		DxObject::StructuredBuffer* verticesStructuredBuffer, DxObject::StructuredBuffer* indicesStructuredBuffer) = 0;

	virtual void SetOnImGui(int id) = 0;

	void SetOnTLAS(DxrObject::TopLevelAS* tlas);

	bool IsDelete() const { return isDelete_; }

protected:

	////////////////////////////////////////////////////////////////////////////////////////////
	// SubobjectMaterial structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct SubobjectMaterial {
		Vector4f color = { 0.0f, 1.0f, 0.0f, 1.0f };
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

};