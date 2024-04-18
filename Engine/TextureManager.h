#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// DirectX12
#include <d3d12.h>
#include "externals/DirectXTex/DirectXTex.h"
#include "externals/DirectXTex/d3dx12.h"

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

	//! @brief コンストラクタ
	Texture(const std::string& filePath, DirectXCommon* dxCommon) { Load(filePath, dxCommon); }

	//! @brief コンストラクタ
	//! @brief ダミーテクスチャ用
	Texture(int32_t width, int32_t height) { CreateDummy(width, height); }

	//! @brief デストラクタ
	~Texture() { Unload(); }

	//! @brief テクスチャのロード
	//! 
	//! @param[in] filePath ファイルパス
	void Load(const std::string& filePath, DirectXCommon* dxCommon);
	
	//! @brief テクスチャの解放
	void Unload();

	void CreateDummy(int32_t width, int32_t height);

	//! @brief テクスチャのGPUハンドルを取得
	//! 
	//! @return テクスチャのGPUハンドルを返却
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetHandle() const { return handleGPU_; }

	const D3D12_CPU_DESCRIPTOR_HANDLE& GetRTVHandleCPU() const { return rtv_.handleCPU; }

	ID3D12Resource* GetResource() const { return textureResource_.Get(); }

private:

	ComPtr<ID3D12Resource>      textureResource_;
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU_;
	uint32_t                    descriptorIndex_;

	DxObject::Descriptor rtv_;

	DirectXCommon* dxCommon_;
};

////////////////////////////////////////////////////////////////////////////////////////////
// TextureManager class
////////////////////////////////////////////////////////////////////////////////////////////
class TextureManager {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief コンストラクタ
	TextureManager() {}

	//! @brief デストラクタ
	~TextureManager() {}

	//! @brief 初期化処理
	void Init(DirectXCommon* dxCommon);

	//! @brief 終了処理
	void Term();

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetHandleGPU(const std::string& filePath) const {
		auto it = textures_.find(filePath);
		assert(it != textures_.end());

		return it->second.texture->GetHandle();
	}

	const Texture* GetTexture(const std::string& key) const {
		auto it = textures_.find(key);
		assert(it != textures_.end());

		return it->second.texture.get();
	}

	void LoadTexture(const std::string& filePath);

	void UnloadTexture(const std::string& filePath);

	void CreateDummyTexture(int32_t width, int32_t height, std::string key);

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