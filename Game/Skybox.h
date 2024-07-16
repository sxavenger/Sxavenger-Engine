#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// DxObject
#include <DxGraphicsBlob.h>
#include <DxGraphicsPipeline.h>

// DrawMethod
#include <DrawMethod.h>

// c++
#include <memory>

// Attribute
#include <Attribute.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Skybox class
////////////////////////////////////////////////////////////////////////////////////////////
class Skybox
	: public Attribute {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Skybox() { Init(); }

	~Skybox() { Term(); }

	void Init();

	void Term();

	void Draw();

	void SetAttributeImGui() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* Graphics *//
	std::unique_ptr<DxObject::GraphicsBlob>     blob_;
	std::unique_ptr<DxObject::GraphicsPipeline> pipeline_;

	//* IA *//
	DrawData skybox_;

	//* Buffer *//
	std::unique_ptr<DxObject::BufferResource<Matrix4x4>> matrixBuffer_;
	std::unique_ptr<DxObject::BufferPtrResource<float>> vignetteBuffer_;
	float strength_ = 1.0f;

};