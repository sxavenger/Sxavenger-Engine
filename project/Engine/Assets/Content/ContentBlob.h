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
//! @brief シェーダーファイルをコンパイルし, シェーダーBlobとして保持するContent
class ContentBlob final
	: public BaseContent {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	ContentBlob() : BaseContent(Async::Execution::Cpu) {}

	~ContentBlob() override = default;

	//* content option *//

	//! @brief シェーダーをコンパイルしBlobを生成する (BaseContentのoverride)
	void Load(MAYBE_UNUSED const DirectXQueueContext* context) override;

	//* parameter option *//

	//! @brief コンパイル済みシェーダーBlobを取得する
	//! @return シェーダーBlobへの参照
	const DxObject::ShaderBlob& GetBlob() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	DxObject::ShaderBlob blob_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	DxObject::CompileProfile GetProfile() const;

};

SXAVENGER_ENGINE_NAMESPACE_END
