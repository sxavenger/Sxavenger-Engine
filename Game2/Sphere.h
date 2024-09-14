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

////////////////////////////////////////////////////////////////////////////////////////////
// Sphere class
////////////////////////////////////////////////////////////////////////////////////////////
class Sphere
	: public Attribute {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Sphere() { Init(); }

	~Sphere() { Term(); }

	void Init();

	void Term();

	void Update();

	void Draw();

	void SetAttributeImGui() override;

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// SphereMaterial structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct SphereMaterial {
		Vector4f color;
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	// mesh
	std::unique_ptr<DxObject::Mesh> mesh_;

	// materialResource
	std::unique_ptr<DxObject::BufferPtrResource<SphereMaterial>> materialResource_;
	SphereMaterial material_;

	// matrixResource
	std::unique_ptr<DxObject::BufferResource<TransformationMatrix>> matrixResource_;
	Transform transform_;

};