#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DxObject
#include <DxBufferResource.h>

//* ObjectStructure
#include <ObjectStructure.h>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// Render3DObject class
////////////////////////////////////////////////////////////////////////////////////////////
class Render3DObject {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Render3DObject()          = default;
	virtual ~Render3DObject() = default;

	void Create(); //!< vertices, indicesの生成

	//* shaderBlockに合わせた描画方法の提供

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	std::unique_ptr<DxObject::BufferResource<VertexData>> vertices_;
	std::unique_ptr<DxObject::IndexBufferResource>        indices_;

};