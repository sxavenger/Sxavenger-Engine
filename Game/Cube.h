#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <Attribute.h>

// c++
#include <memory>

// Geometry
#include <Vector4.h>

// DxObject
#include <DxBufferResource.h>
#include <DxMesh.h>

#include <Model.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sphere class
////////////////////////////////////////////////////////////////////////////////////////////
class Cube
	: public Attribute {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Cube() { Init(); }

	~Cube() { Term(); }

	void Init();

	void Term();

	void Update();

	void Draw();

	void SetAttributeImGui() override;

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// SphereMaterial structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct CubeMaterial {
		Vector4f color;
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	// mesh
	std::unique_ptr<DxObject::Mesh> mesh_;

	// materialResource
	std::unique_ptr<DxObject::BufferPtrResource<CubeMaterial>> materialResource_;
	CubeMaterial material_;

	// matrixResource
	std::unique_ptr<DxObject::BufferResource<TransformationMatrix>> matrixResource_;
	Transform transform_;

};