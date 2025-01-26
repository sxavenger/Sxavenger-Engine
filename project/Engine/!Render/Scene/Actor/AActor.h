#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Module/Component/TransformComponent.h>

//* c++
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////
// AActor class
////////////////////////////////////////////////////////////////////////////////////////////
class AActor
	: public TransformComponent {
	//* 3D Object で Transformを持っているモノをActorとして扱う
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	AActor()          = default;
	virtual ~AActor() = default;

	virtual void InspectorImGui() {}
	//!< Inspector表示用のImGui

	const std::string& GetName() const { return name_; }
	std::string& GetName() { return name_; }

	void SetName(const std::string& name) { name_ = name; }

	bool IsActive() const { return isActive_; }
	bool& GetIsActive() { return isActive_; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	std::string name_ = "actor";

	bool isActive_ = true;

};
