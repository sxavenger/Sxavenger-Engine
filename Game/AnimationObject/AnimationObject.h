#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// DxObject
#include <DxGraphicsBlob.h>
#include <DxGraphicsPipeline.h>
#include <DxBufferResource.h>

// c++
#include <memory>

// Attribute
#include <Attribute.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AnimationObject Base class
////////////////////////////////////////////////////////////////////////////////////////////
class AnimationObject
	: public Attribute {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	AnimationObject() = default;
	virtual ~AnimationObject() = default;
	
	virtual void SetAttributeImGui();

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* Graphics *//
	std::unique_ptr<DxObject::GraphicsBlob>     blob_;
	std::unique_ptr<DxObject::GraphicsPipeline> pipeline_;

	//=========================================================================================
	// protected methods
	//=========================================================================================

	void CreateGraphicsPipeline();

	void SetPipeline();

};