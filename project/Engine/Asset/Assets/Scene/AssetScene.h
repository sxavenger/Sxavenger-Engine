#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* asset
#include "../BaseAsset.h"

//* lib
#include <Lib/Adapter/Json/JsonHandler.h>

//////////////////////////////////////////////////////////////////////////////////////////////
// Asset Scene class
//////////////////////////////////////////////////////////////////////////////////////////////
class AssetScene
	: public BaseAsset {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	AssetScene()  = default;
	~AssetScene() = default;

	//* async task option *//

	void Load(_MAYBE_UNUSED const DirectXQueueContext* context) override;

	AsyncExecution GetAsyncExecution() const override { return AsyncExecution::None; }

	//* inspector option *//

	void ShowInspector() override;

	//* asset option *//

	const json& GetData() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	json data_;

};
