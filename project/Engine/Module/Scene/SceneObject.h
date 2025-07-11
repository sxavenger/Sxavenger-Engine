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

	SceneObject()  = default;
	~SceneObject() = default;

	//* asset option *//

	void CreateMeshComponent(const std::filesystem::path& filepath);

	//* behaviour option *//

	void Inspectable() override;

	//* serialize option *//

	json PerseToJson() const override;

	void InputJson(const json& data) override;

	bool IsDelete() const { return isDelete_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::filesystem::path filepath_ = "";

	bool isDelete_ = false;
	
};
