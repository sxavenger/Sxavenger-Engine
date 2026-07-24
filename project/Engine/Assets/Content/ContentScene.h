#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* content
#include "BaseContent.h"

//* engine
#include <Engine/Foundation.h>

//* lib
#include <Lib/Adapter/Json/JsonHandler.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// ContentScene class
////////////////////////////////////////////////////////////////////////////////////////////
class ContentScene final
	: public BaseContent {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	ContentScene() : BaseContent(Async::Execution::Cpu) {}

	~ContentScene() override = default;

	//* content option *//

	void Load(MAYBE_UNUSED const DirectXQueueContext* context) override;

	//* parameter option *//

	json GetData() const { return data_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	json data_;

};

SXAVENGER_ENGINE_NAMESPACE_END
