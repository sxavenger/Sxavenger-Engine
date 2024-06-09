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

	//=========================================================================================
	// protected variables
	//=========================================================================================

	bool isDelete_ = false;

};