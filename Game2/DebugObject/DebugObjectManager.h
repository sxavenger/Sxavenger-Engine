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
#include <DxShaderBlob.h>
#include <DxGraphicsPipeline.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DebugObjectManager class
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

	////////////////////////////////////////////////////////////////////////////////////////////
	// DebugObjectType enum
	////////////////////////////////////////////////////////////////////////////////////////////
	enum DebugObjectType : uint32_t {
		kSphere,
		kBox,

		kCountOfDebugObjectType
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* member *//

	std::list<std::unique_ptr<DebugObject>> debugObjects_;
	DebugObjectType selectedObjectType_ = DebugObjectType::kSphere;
	static const LPCSTR kObjectNames_[DebugObjectType::kCountOfDebugObjectType];

	//* Graphics *//

	std::unique_ptr<DxObject::GraphicsBlob>     blob_;
	std::unique_ptr<DxObject::GraphicsPipeline> pipeline_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreatePipeline();

	void CreateObject();

};