#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// debugObject base
#include <DebugObject.h>

// c++
#include <list>
#include <memory>

// Graphics
#include <DxGraphicsBlob.h>
#include <DxGraphicsPipeline.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DebugObject base class
////////////////////////////////////////////////////////////////////////////////////////////
class DebugObjectManager
	: public Attribute {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	DebugObjectManager() = default;
	~DebugObjectManager() = default;

	void Init();

	void Update();

	void Draw();

	void SetAttributeImGui() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* member *//

	std::list<std::unique_ptr<DebugObject>> debugObjects_;

	//* Graphics *//

	std::unique_ptr<DxObject::GraphicsBlob>     blob_;
	std::unique_ptr<DxObject::GraphicsPipeline> pipeline_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreatePipeline();

};