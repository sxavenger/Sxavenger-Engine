#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* ucontent
#include "UBaseContent.h"

//* lib
#include <Lib/Adapter/Uuid/Uuid.h>

//* directx12
#include <d3dx12.h>
#include <DirectXTex.h>

////////////////////////////////////////////////////////////////////////////////////////////
// UContentFont class
////////////////////////////////////////////////////////////////////////////////////////////
class UContentFont final
	: public UBaseContent {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	UContentFont()           = default;
	~UContentFont() override = default;

	void AsyncLoad(_MAYBE_UNUSED const DirectXQueueContext* context) override;

	AsyncExecution GetAsyncExecution() const { return AsyncExecution::Compute; }

	void AttachUuid() override;

	//* inspector option *//

	void ShowInspector() override;

	//* content option *//

	void Load(const DirectXQueueContext* context, const std::filesystem::path& filepath);

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
