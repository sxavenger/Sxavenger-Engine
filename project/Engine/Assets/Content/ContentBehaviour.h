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
// ContentBehaviour class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief Behaviour定義のjsonファイルを読み込み保持するContent
class ContentBehaviour final
	: public BaseContent {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	ContentBehaviour() : BaseContent(Async::Execution::Cpu) {}

	~ContentBehaviour() override = default;

	//* content option *//

	//! @brief jsonファイルを読み込む (BaseContentのoverride)
	void Load(MAYBE_UNUSED const DirectXQueueContext* context) override;

	//* parameter option *//

	//! @brief 読み込んだjsonデータを取得する
	//! @return jsonデータへの参照
	const json& GetData() const { return data_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	json data_;

};

SXAVENGER_ENGINE_NAMESPACE_END
