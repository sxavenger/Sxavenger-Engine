#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* ucontent
#include "UBaseContent.h"

//* lib
//#include <Lib/Adapter/Uuid/Uuid.h>
#include <Lib/Adapter/Json/JsonHandler.h>

////////////////////////////////////////////////////////////////////////////////////////////
// UContentBlob class
////////////////////////////////////////////////////////////////////////////////////////////
class UContentScene final
	: public UBaseContent {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	UContentScene()           = default;
	~UContentScene() override = default;

	void AsyncLoad(_MAYBE_UNUSED const DirectXQueueContext* context) override;

	AsyncExecution GetAsyncExecution() const { return AsyncExecution::None; }

	void AttachUuid() override;

	//* content option *//

	void Load(const std::filesystem::path& filepath);

	const json& GetData() const { return data_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	json data_;

};
