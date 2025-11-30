#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* ucontent
#include "BaseContent.h"

//* lib
#include <Lib/Adapter/Json/JsonHandler.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ContentBlob class
////////////////////////////////////////////////////////////////////////////////////////////
class ContentScene final
	: public BaseContent {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ContentScene()           = default;
	~ContentScene() override = default;

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
