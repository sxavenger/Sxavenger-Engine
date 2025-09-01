#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* uasset
#include "UBaseAsset.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxDescriptor.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

//* lib
#include <Lib/Adapter/Uuid/Uuid.h>

//* external
#include <stb_truetype.h>

//* c++
#include <unordered_map>

////////////////////////////////////////////////////////////////////////////////////////////
// UAssetFont class
////////////////////////////////////////////////////////////////////////////////////////////
class UAssetFont final
	: public UBaseAsset {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// GlyphInfo structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct GlyphInfo {
	public:

		//=========================================================================================
		// public variables
		//=========================================================================================

		Vector2f uv[2];
		Vector2f size;
		Vector2f offset;
		float advance;
		
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	UAssetFont(const Uuid& id) : UBaseAsset(id) {}
	~UAssetFont() override = default;

	void Setup(const DirectXQueueContext* context, const stbtt_fontinfo& info, float size);

	//* inspector *//

	void ShowInspector() override;

	//* getter *//

	const DxObject::Descriptor& GetDescriptorSRV() const;

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleSRV() const;

	float GetFontSize() const { return fontSize_; }

	const GlyphInfo& GetGlyphInfo(wchar_t c) const;



private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* directx12 *// 

	ComPtr<ID3D12Resource> resource_;
	DxObject::Descriptor   descriptorSRV_;

	//* font intermediate *//

	Vector2i current_  = {};
	int32_t maxHeight_ = 0;

	std::vector<uint8_t> atlasData_;
	//!< todo: 中間データとして持たせるので削除する

	//* parameter *//

	static inline const Vector2ui kAtlasSize = { 1024, 1024 };

	float fontSize_ = NULL;

	int32_t ascent_  = NULL;
	int32_t descent_ = NULL;

	std::unordered_map<wchar_t, GlyphInfo> glyphs_;
	//!< wchar_tごとのglyph情報

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* create helper methods *//

	void CreateAtlasTexture();

	void LoadGlyph(const stbtt_fontinfo& info, float scale);

	GlyphInfo GenerateGlyphInfo(const stbtt_fontinfo& info, float scale, wchar_t c);

	void UploadAtlasData(const DirectXQueueContext* context);

	//* imgui helper methods *//

	void ShowTexture();

};
