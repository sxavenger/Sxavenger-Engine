#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* asset
#include "BaseAsset.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/Utility/ComPtr.h>

//* mf
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>

//* c++
#include <vector>

//-----------------------------------------------------------------------------------------
// comment
//-----------------------------------------------------------------------------------------
//* mf
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "Mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// AssetAudioClip class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief Media Foundationで音声を読み込み, 波形フォーマットとPCMデータを保持するAsset
class AssetAudioClip final
	: public BaseAsset {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	AssetAudioClip(const Uuid& id) : BaseAsset(id) {}

	~AssetAudioClip() override = default;

	//* setup option *//

	//! @brief Media FoundationのソースリーダーからPCMデータを読み込む
	//! @param[in] reader デコード済み音声を供給するソースリーダー
	void Setup(IMFSourceReader* reader);

	//* audio clip option *//

	//! @brief 波形フォーマットを取得する
	//! @return WAVEFORMATEXへの参照
	const WAVEFORMATEX& GetFormat() const;

	//! @brief 波形データ先頭へのポインタを取得する
	//! @return PCMデータの先頭ポインタ
	const BYTE* GetBufferPointer() const;

	//! @brief 波形データのバイトサイズを取得する
	//! @return PCMデータのサイズ(byte)
	const size_t GetBufferSize() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	WAVEFORMATEX format_ = {}; //!< 波形フォーマット
	std::vector<BYTE> buffer_; //!< 波形データ

};

SXAVENGER_ENGINE_NAMESPACE_END
