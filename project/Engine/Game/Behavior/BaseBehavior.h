#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DxrObject/DxrAccelerationStructure.h>

//* lib
#include <Lib/CXXAttributeConfig.h>

//* c++
#include <list>
#include <string>

//-----------------------------------------------------------------------------------------
// Forward
//-----------------------------------------------------------------------------------------
class Camera3D;

////////////////////////////////////////////////////////////////////////////////////////////
// BehaviorRenderingType enum
////////////////////////////////////////////////////////////////////////////////////////////
enum BehaviorRenderingFlag : uint32_t {
	kBehaviorRender_None         = 0,
	kBehaviorRender_Systematic   = 1 << 0,
	kBehaviorRender_Adaptive     = 1 << 1,
	kBehaviorRender_LateAdaptive = 1 << 1,
	kBehaviorRender_Raytracing   = 1 << 2,
};

////////////////////////////////////////////////////////////////////////////////////////////
// BaseBehavior class
////////////////////////////////////////////////////////////////////////////////////////////
class BaseBehavior {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	BaseBehavior()          = default;
	virtual ~BaseBehavior() { Term(); }

	//* behavior option *//

	void SetToConsole();
	void SetToConsole(const std::string& name);

	void SetName(const std::string& name) { name_ = name; }

	void SetChild(BaseBehavior* child);
	void RemoveChild(BaseBehavior* child);

	//* derivative behaivor methods *//
	//* imgui command
	virtual void SystemAttributeImGui() {} //!< system using
	virtual void SetAttributeImGui() {}    //!< userが定義できるimgui command

	//* draw
	virtual void DrawSystematic(_MAYBE_UNUSED const Camera3D* camera) {}
	virtual void DrawAdaptive(_MAYBE_UNUSED const Camera3D* camera) {}
	//virtual void DrawLateAdaptive(_MAYBE_UNUSED const Camera3D* camera) {}
	virtual void DrawRaytracing(_MAYBE_UNUSED DxrObject::TopLevelAS* tlas) {}


	virtual void DrawLocal() {} //!< local sceneへの描画用

	//* behavior getter *//

	const std::string& GetName() const { return name_; }

	const uint32_t GetRenderingFlag() const { return renderingFlag_; }

	const std::list<BaseBehavior*>& GetChildren() const { return children_; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* behavior parameter *//

	std::string name_ = "new behavior";

	uint32_t renderingFlag_ = kBehaviorRender_None;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* node system *//
	//!< nodeは親がdeleteされる場合, 子から先にdeleteする.

	std::list<BaseBehavior*> children_;
	BaseBehavior*            parent_ = nullptr;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void Term();

};