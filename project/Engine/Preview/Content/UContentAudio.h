#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* ucontent
#include "UBaseContent.h"

////////////////////////////////////////////////////////////////////////////////////////////
// UContentAudio class
////////////////////////////////////////////////////////////////////////////////////////////
class UContentAudio
	: public UBaseContent {
public:

	UContentAudio()           = default;
	~UContentAudio() override = default;

	void AsyncLoad(_MAYBE_UNUSED const DirectXQueueContext* context) override;

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
