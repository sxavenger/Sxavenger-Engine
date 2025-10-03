#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* ucontent
#include "UBaseContent.h"

//* lib
//#include <Lib/Adapter/Uuid/Uuid.h>

//* engine
#include <Engine/System/DirectX/DxObject/DxShaderBlob.h>

////////////////////////////////////////////////////////////////////////////////////////////
// UContentBlob class
////////////////////////////////////////////////////////////////////////////////////////////
class UContentBlob final
	: public UBaseContent {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	UContentBlob() = default;
	~UContentBlob() override = default;

	void AsyncLoad(_MAYBE_UNUSED const DirectXQueueContext* context) override;

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
