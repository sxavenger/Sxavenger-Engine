#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Content/Component/TransformComponent.h>

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

	//* getter *//

	const std::string& GetName() const { return name_; }
	std::string& GetName() { return name_; }

	bool IsActive() const { return isActive_; }

	//* setter *//

	void SetActive(bool isActive) { isActive_ = isActive; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	std::string name_ = "actor";

	bool isActive_ = true;

};
