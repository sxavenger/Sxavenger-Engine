#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// c++
#define _USE_MATH_DEFINES
#include <cmath>
// MyEngine
#include "MyEngine.h" //!< デバイス取り出し

// DxObject
#include <DxBufferResource.h>

// structure
#include <ObjectStructure.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DrawData structure
////////////////////////////////////////////////////////////////////////////////////////////
struct DrawData {
	DxObject::BufferResource<VertexData>* vertex;
	DxObject::IndexBufferResource*        index;
};

////////////////////////////////////////////////////////////////////////////////////////////
// DrawMethod namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace DrawMethods {
	
	DxObject::BufferResource<VertexData>* Sphere(float radius, uint32_t kSubdivision);

	DrawData SphereData(float radius, uint32_t kSubdivision);

};