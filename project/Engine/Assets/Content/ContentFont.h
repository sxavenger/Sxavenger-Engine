#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* content
#include "BaseContent.h"

//* engine
#include <Engine/Foundation.h>

//* lib
#include <Lib/Adapter/Uuid/Uuid.h>

//* directx12
#include <d3dx12.h>
#include <DirectXTex.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// ContentFont class
////////////////////////////////////////////////////////////////////////////////////////////
class ContentFont final
	: public BaseContent {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ContentFont()           = default;
	~ContentFont() override = default;

	void AsyncLoad(MAYBE_UNUSED const DirectXQueueContext* context) override;

	AsyncExecution GetAsyncExecution() const { return AsyncExecution::Compute; }

	void AttachUuid() override;

	//* inspector option *//

	void ShowInspector() override;

	//* content option *//

	void Load(const DirectXQueueContext* context, const std::filesystem::path& filepath);

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
