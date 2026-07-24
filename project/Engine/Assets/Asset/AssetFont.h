#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* uasset
#include "BaseAsset.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/DxObject/DxResource.h>
#include <Engine/System/DirectX/DxObject/DxDescriptor.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

//* lib
#include <Lib/Adapter/Uuid/Uuid.h>

//* external
#include <stb_truetype.h>

//* c++
#include <unordered_map>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// AssetFont class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief stb_truetypeでフォントをラスタライズし, グリフアトラステクスチャとして保持するAsset
class AssetFont final
	: public BaseAsset {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// GlyphInfo structure
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief 1文字分のアトラス上のUV/サイズ/オフセット/送り幅を保持する構造体
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

	AssetFont(const Uuid& id) : BaseAsset(id) {}

	~AssetFont() override = default;

	//* setup option *//

	//! @brief フォント情報からグリフを生成し, アトラステクスチャをGPUへ構築する
	//! @param[in] context DirectXのキューコンテキスト
	//! @param[in] info    stb_truetypeのフォント情報
	//! @param[in] size    ラスタライズするフォントサイズ(px)
	void Setup(const DirectXQueueContext* context, const stbtt_fontinfo& info, float size);

	//* font option *//

	//! @brief アトラステクスチャのSRV Descriptorを取得する
	//! @return SRVのDescriptorへの参照
	const DxObject::Descriptor& GetDescriptorSRV() const;

	//! @brief アトラステクスチャのSRV GPUハンドルを取得する
	//! @return SRVのGPUディスクリプタハンドルへの参照
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleSRV() const;

	//! @brief ラスタライズしたフォントサイズを取得する
	//! @return フォントサイズ(px)
	float GetFontSize() const { return fontSize_; }

	//! @brief 指定文字のグリフ情報を取得する
	//! @param[in] c 対象の文字
	//! @return グリフ情報への参照
	const GlyphInfo& GetGlyphInfo(wchar_t c) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* directx12 *// 

	DxObject::Resource   resource_;
	DxObject::Descriptor descriptorSRV_;

	//* font intermediate *//

	Vector2i current_ = {};
	int32_t maxHeight_ = 0;

	std::vector<uint8_t> atlasData_;
	//!< todo: 中間データとして持たせるので削除する

	//* parameter *//

	static inline const Vector2ui kAtlasSize = { 1024, 1024 };

	float fontSize_ = NULL;

	int32_t ascent_ = NULL;
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

};

SXAVENGER_ENGINE_NAMESPACE_END
