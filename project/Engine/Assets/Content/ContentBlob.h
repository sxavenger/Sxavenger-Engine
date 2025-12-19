#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* content
#include "BaseContent.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/DxObject/DxShaderBlob.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// ContentBlob class
////////////////////////////////////////////////////////////////////////////////////////////
class ContentBlob final
	: public BaseContent {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ContentBlob() = default;
	~ContentBlob() override = default;

	void AsyncLoad(MAYBE_UNUSED const DirectXQueueContext* context) override;

	AsyncExecution GetAsyncExecution() const { return AsyncExecution::None; }

	void AttachUuid() override;

	//* content option *//

	void Load(const std::filesystem::path& filepath, const DxObject::CompileProfile& profile);

	const DxObject::ShaderBlob& GetBlob() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	DxObject::ShaderBlob blob_;

};

SXAVENGER_ENGINE_NAMESPACE_END
