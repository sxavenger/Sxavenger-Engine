#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* asset
#include "BaseAsset.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/DxObject/DxResource.h>
#include <Engine/System/DirectX/DxObject/DxDescriptor.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

//* lib
#include <Lib/Math/Vector2.h>

//* directx12
#include <d3dx12.h>
#include <DirectXTex.h>

//* c++
#include <array>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// AssetTexture class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief テクスチャ画像をGPUリソース化し, SRVと共に保持するAsset
class AssetTexture final
	: public BaseAsset {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Metadata structure
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief テクスチャの解像度やフォーマットなどのメタ情報を保持する構造体
	struct Metadata {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//! @brief DirectXTexのメタデータから各パラメータを設定する
		//! @param[in] metadata DirectXTexが読み込んだメタデータ
		void Assign(const DirectX::TexMetadata& metadata);

		//! @brief フォーマットからカラーエンコーディング(sRGB/リニア)を判定する
		//! @return sRGBならLightness, それ以外はIntensity
		DxObject::ColorEncoding GetColorEncoding() const {
			return DirectX::IsSRGB(format) ? DxObject::ColorEncoding::Lightness : DxObject::ColorEncoding::Intensity;
		}

		//! @brief キューブマップかどうかを判定する
		//! @retval true  キューブマップ
		//! @retval false キューブマップではない
		bool IsCubemap() const { return (miscflags[0] & DirectX::TEX_MISC_TEXTURECUBE) != 0; }

		//=========================================================================================
		// public variables
		//=========================================================================================

		Vector2ui size;
		uint32_t depth;

		uint32_t miplevels;
		DXGI_FORMAT format;

		std::array<uint32_t, 2> miscflags;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	AssetTexture(const Uuid& id) : BaseAsset(id) {}

	~AssetTexture() override = default;

	//* setup option *//

	//! @brief 画像データからGPUテクスチャリソースとSRVを生成する
	//! @param[in] context DirectXのキューコンテキスト
	//! @param[in] image   DirectXTexで読み込んだ画像データ
	void Setup(const DirectXQueueContext* context, const DirectX::ScratchImage& image);

	//* texture option *//

	//! @brief テクスチャリソースを描画で利用可能な状態(PixelShaderResource)へ遷移させる
	//! @param[in] context DirectXのキューコンテキスト
	void Transition(const DirectXQueueContext* context);

	//! @brief SRVのDescriptorを取得する
	//! @return SRVのDescriptorへの参照
	const DxObject::Descriptor& GetDescriptorSRV() const;

	//! @brief SRVのGPUディスクリプタハンドルを取得する
	//! @return SRVのGPUディスクリプタハンドルへの参照
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleSRV() const;

	//! @brief テクスチャのメタ情報を取得する
	//! @return メタデータへの参照
	const Metadata& GetMetadata() const { return metadata_; }
	// hack: metadata読み込みの完了を待つ必要がある

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* DirectX12 *// 

	DxObject::Resource   resource_;
	DxObject::Descriptor descriptorSRV_;

	//* parameter *//

	Metadata metadata_ = {};

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* texture helper methods *//

	static DxObject::Resource CreateTextureResource(const DirectX::TexMetadata& metadata);

	static NODISCARD ComPtr<ID3D12Resource> UploadTextureData(const DirectXQueueContext* context, ID3D12Resource* texture, const DirectX::ScratchImage& image);

};

SXAVENGER_ENGINE_NAMESPACE_END
