#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* uasset
#include "BaseAsset.h"

//* engine
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
// AssetAudioClip class
////////////////////////////////////////////////////////////////////////////////////////////
class AssetAudioClip final
	: public BaseAsset {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	AssetAudioClip(const Uuid& id) : BaseAsset(id) {}
	~AssetAudioClip() override = default;

	void Setup(IMFSourceReader* reader);

	//* inspector *//

	void ShowInspector() override;

	//* getter *//

	const WAVEFORMATEX& GetFormat() const;

	const BYTE* GetBufferPointer() const;

	const size_t GetBufferSize() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	WAVEFORMATEX format_ = {}; //!< 波形フォーマット
	std::vector<BYTE> buffer_; //!< 波形データ

};	
