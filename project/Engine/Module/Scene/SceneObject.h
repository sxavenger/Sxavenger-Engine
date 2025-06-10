#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Component/Entity/MonoBehaviour.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SceneObject class
////////////////////////////////////////////////////////////////////////////////////////////
class SceneObject
	: public MonoBehaviour {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* asset option *//

	void CreateMeshComponent(const std::filesystem::path& filepath);

	//* serialize option *//

	json PerseToJson() const override;

	void InputJson(const json& data) override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::filesystem::path filepath_ = "";

	bool isDelete_ = false;
	
};
