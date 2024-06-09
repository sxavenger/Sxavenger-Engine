#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// subobject
#include <Subobject.h>

// model
#include <Model.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Cube class
////////////////////////////////////////////////////////////////////////////////////////////
class Cube
	: public Subobject {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Cube() {};

	~Cube() override = default;

	void Init(
		DxObject::BufferResource<VertexData>* vertices, DxObject::IndexBufferResource* indices,
		DxObject::StructuredBuffer* verticesStructuredBuffer, DxObject::StructuredBuffer* indicesStructuredBuffer
	) override;

	void SetOnImGui(int id) override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

};