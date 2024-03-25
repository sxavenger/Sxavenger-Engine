#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// c++
#include <memory>

#include <DxBufferResource.h>

#include <ObjectStructure.h>
#include <DrawMethod.h>

#include "MyEngine.h"

#include <Light.h>

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

	void Draw(Light* light);

	void Term();

	void SetOnImGui();

private:

	//=========================================================================================
	// private varibles
	//=========================================================================================

	DrawData data_;

	// resource
	std::unique_ptr<DxObject::BufferResource<TransformationMatrix>> matrixResource_;
	Transform transform_;

	std::unique_ptr<DxObject::BufferPtrResource<Material>> materialResource_;
	Material material_;
	
};