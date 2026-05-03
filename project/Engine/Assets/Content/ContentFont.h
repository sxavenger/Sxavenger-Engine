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

	//* constructor / destructor *//

	ContentFont() : BaseContent(Async::Execution::Copy) {}

	~ContentFont() override = default;

	//* content option *//

	void Attach(const std::filesystem::path& filepath, const std::any& parameter) override;

	void Load(MAYBE_UNUSED const DirectXQueueContext* context) override;

	//* id option *//

	const Uuid& GetId() const { return id_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================
	
	Uuid id_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void AttachUuid(const std::filesystem::path& filepath);

};

SXAVENGER_ENGINE_NAMESPACE_END
