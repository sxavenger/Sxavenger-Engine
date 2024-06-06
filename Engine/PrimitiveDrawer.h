#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// DxObject
#include <DxBufferResource.h>

// camera

// Geometry
#include <Vector4.h>
#include <Matrix4x4.h>

// c++
#include <memory>
#include <array>

// Object
#include <ObjectStructure.h>

#include <MyEngine.h>


////////////////////////////////////////////////////////////////////////////////////////////
// PrimitiveDrawer class
////////////////////////////////////////////////////////////////////////////////////////////
class PrimitiveDrawer {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init();

	void Term();

	void DrawAll();

	void DrawTriangle(const Vector3f& w1, const Vector3f& w2, const Vector3f& w3, uint32_t color);

	static PrimitiveDrawer* GetInstance();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	

	// MyEngineからカメラを持ってくるとする

};