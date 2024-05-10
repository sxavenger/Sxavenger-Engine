#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// DirectX12
#include <d3d12.h>
#include "DirectXTex.h"
#include "d3dx12.h"

#include <DxDescriptorHeaps.h>

// c++
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

// ComPtr
#include <ComPtr.h>

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class DirectXCommon;

////////////////////////////////////////////////////////////////////////////////////////////
// Texture class
////////////////////////////////////////////////////////////////////////////////////////////
class Texture {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief texture用コンストラクタ
	Texture(const std::string& filePath, DirectXCommon* dxCommon) {
		Load(filePath, dxCommon);
	}

	//! @brief dummyTexture用コンストラクタ
	Texture(int32_t width, int32_t height, DirectXCommon* dxCommon) {
		CreateDummy(width, height, dxCommon);
	}

	//! @brief デストラクタ
	~Texture() { Unload(); }

	//! @brief テクスチャのロード
	//! 
	//! @param[in] filePath ファイルパス
	void Load(const std::string& filePath, DirectXCommon* dxCommon);

	//! @brief ダミーテクスチャの生成
	void CreateDummy(int32_t width, int32_t height, DirectXCommon* dxCommon);

	//! @brief テクスチャの解放
	void Unload();

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetSRVHandleGPU() const { return descriptorSRV_.handleGPU; }

	const D3D12_CPU_DESCRIPTOR_HANDLE& GetRTVHandleCPU() const { return descriptorRTV_.handleCPU; }

	ID3D12Resource* GetResource() const { return resource_.Get(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	DirectXCommon* dxCommon_ = nullptr;

	ComPtr<ID3D12Resource> resource_;

	DxObject::Descriptor descriptorSRV_;
	DxObject::Descriptor descriptorRTV_; //!< ダミーテクスチャ用

};

////////////////////////////////////////////////////////////////////////////////////////////
// TextureManager class
////////////////////////////////////////////////////////////////////////////////////////////
class TextureManager {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief 初期化処理
	void Init(DirectXCommon* dxCommon);

	//! @brief 終了処理
	void Term();

	//! @brief テクスチャのロード
	void LoadTexture(const std::string& filePath);

	//! @brief ダミーテクスチャの設定
	void CreateDummyTexture(int32_t width, int32_t height, std::string key);

	//! @brief テクスチャのアンロード
	void UnloadTexture(const std::string& key);

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetHandleGPU(const std::string& key) const {
		auto it = FindKey(key);

		return it->second.texture->GetSRVHandleGPU();
	}

	Texture* GetTexture(const std::string& key) const {
		auto it = FindKey(key);

		return it->second.texture.get();
	}

	static TextureManager* GetInstance();

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// TextureData structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct TextureData {
		std::unique_ptr<Texture> texture;
		uint32_t referenceNum = 0;
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unordered_map<std::string, TextureData> textures_;
	//!< key = filePath, value = texture

	DirectXCommon* dxCommon_ = nullptr;

	//=========================================================================================
	// private methods
	//=========================================================================================

	std::unordered_map<std::string, TextureManager::TextureData>::const_iterator FindKey(const std::string& key) const;

};

////////////////////////////////////////////////////////////////////////////////////////////
// TextureMethod namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace TextureMethod {

	DirectX::ScratchImage LoadTexture(const std::string& filePath);

	ComPtr<ID3D12Resource> CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata);

	[[nodiscard]]
	ComPtr<ID3D12Resource> UploadTextureData(
		ID3D12Resource* texture, const DirectX::ScratchImage& mipImages,
		ID3D12Device* device, ID3D12GraphicsCommandList* commandList
	);

}