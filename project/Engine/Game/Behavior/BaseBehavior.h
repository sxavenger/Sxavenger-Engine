#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
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
	virtual void DrawLocal() {} //!< local sceneへの描画用

	//* behavior getter *//

	const std::string& GetName() const { return name_; }

	const std::list<BaseBehavior*>& GetChildren() const { return children_; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* behavior parameter *//

	std::string name_ = "new behavior";

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