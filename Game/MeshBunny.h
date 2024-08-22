#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// DxObject
// Graphics
#include <DxShaderBlob.h>
#include <DxGraphicsPipeline.h>

// Buffer
#include <DxBufferResource.h>

// Mesh
#include <DxMesh.h>

// c++
#include <memory>

// engine
#include <Model.h>
#include <Camera3D.h>
#include <Transform.h>

////////////////////////////////////////////////////////////////////////////////////////////
// MeshBunny class
////////////////////////////////////////////////////////////////////////////////////////////
class MeshBunny
	: public Attribute {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	MeshBunny() { Init(); }

	~MeshBunny() { Term(); }

	void Init();

	void Term();

	void Draw();

	void SetAttributeImGui() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	// mesh
	std::unique_ptr<DxObject::Mesh> mesh_;
	std::unique_ptr<Model> model_;

	// grahics
	std::unique_ptr<DxObject::GraphicsBlob>     blob_;
	std::unique_ptr<DxObject::GraphicsPipeline> pipeline_;

	// buffers
	std::unique_ptr<DxObject::BufferResource<Vector4f>>             material_;

	std::unique_ptr<DxObject::BufferResource<TransformationMatrix>> matrix_;
	EulerTransform transform_;

	std::unique_ptr<Camera3D> cullingCamera_;

};