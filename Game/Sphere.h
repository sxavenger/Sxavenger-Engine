#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// c++
#include <memory>

#include <DxBufferResource.h>

#include <ObjectStructure.h>
#include <DrawMethod.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sphere class
////////////////////////////////////////////////////////////////////////////////////////////
class Sphere {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Sphere() { Init(); }

	~Sphere() { Term(); }

	void Init();

	void Draw();

	void Term();

	void SetOnImGui();

private:

	//=========================================================================================
	// private varibles
	//=========================================================================================

	DrawData data_;

	// resource
	std::unique_ptr<DxObject::BufferResource<TransformationMatrix>> matrixResource_;
	std::unique_ptr<DxObject::BufferPtrResource<Material>>             materialResource_;

	Material material_;

	static const char* lambertItems_[LambertType::kLambertTypeCount];
	static const char* phongItems_[PhongType::kPhongTypeCount];
	
};