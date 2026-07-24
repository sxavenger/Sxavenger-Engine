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
//! @brief 音声ファイルを読み込み, 音声クリップAsset化するContent
class ContentAudio
	: public BaseContent {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	ContentAudio() : BaseContent(Async::Execution::Cpu) {}

	~ContentAudio() override = default;

	//* content option *//

	//! @brief 読み込み対象ファイルとパラメータを関連付ける (BaseContentのoverride)
	void Attach(const std::filesystem::path& filepath, const std::any& parameter) override;

	//! @brief 音声を読み込み, 音声クリップAssetを生成・登録する (BaseContentのoverride)
	void Load(MAYBE_UNUSED const DirectXQueueContext* context) override;

	//* id option *//

	//! @brief 生成した音声クリップAssetのuuidを取得する
	//! @return 音声クリップAssetのuuid
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
