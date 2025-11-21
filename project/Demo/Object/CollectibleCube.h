#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Module/GameObject/GameObject.h>
#include <Engine/Adapter/Parameter/SerializeParameter.h>
#include <Engine/Adapter/Parameter/SerializeGui.h>
#include <Engine/Preview/Content/ContentModel.h>
#include <Engine/Preview/Content/ContentObserver.h>

////////////////////////////////////////////////////////////////////////////////////////////
// CollectibleCube class
////////////////////////////////////////////////////////////////////////////////////////////
class CollectibleCube
	: public MonoBehaviour {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init();

	void Update();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* behaviour *//

	MonoBehaviour* cube_[2];

	//* asset *//

	ContentObserver<ContentModel> model_;

};
