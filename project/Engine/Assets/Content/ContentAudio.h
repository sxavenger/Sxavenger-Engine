#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* content
#include "BaseContent.h"

//* engine
#include <Engine/Foundation.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// ContentAudio class
////////////////////////////////////////////////////////////////////////////////////////////
class ContentAudio
	: public BaseContent {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ContentAudio()           = default;
	~ContentAudio() override = default;

	void AsyncLoad(MAYBE_UNUSED const DirectXQueueContext* context) override;

	AsyncExecution GetAsyncExecution() const { return AsyncExecution::None; }

	void AttachUuid() override;

	//* content option *//

	void Load(const std::filesystem::path& filepath);

	const Uuid& GetId() const { return id_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Uuid id_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* helper methods *//

	void GetUuid();

};

SXAVENGER_ENGINE_NAMESPACE_END
