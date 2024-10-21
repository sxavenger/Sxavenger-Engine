#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <string>
#include <list>

////////////////////////////////////////////////////////////////////////////////////////////
// BaseBehavior base class
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

	void SetChild(BaseBehavior* child);
	void RemoveChild(BaseBehavior* child);

	void SetName(const std::string& name) { name_ = name; }

	//* behavior getter *//

	const std::string& GetName() const { return name_; }

	const std::list<BaseBehavior*>& GetChildren() const { return children_; }

	//* derivative behavior methods *//
	//* ImGui Commands
	virtual void SystemAttributeImGui() {} //!< behavior define
	virtual void SetAttributeImGui() {}    //!< user define

	//* Draw
	virtual void SystemDraw() {} //!< behavior define
	virtual void Draw() {}       //!< user define

	//* LocalMesh
	virtual void SystemDrawLocalMesh() {} //!< behavior define

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	std::string name_ = "New Behavior";

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* node system *//
	//!< nodeは親がdeleteされる場合, 子から先にdeleteする.
	
	BaseBehavior*            parent_ = nullptr; //!< nullptrの場合, rootとなる.
	std::list<BaseBehavior*> children_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void Term();

};