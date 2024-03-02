#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// DirectX12
#include <d3d12.h>
#include "externals/DirectXTex/DirectXTex.h"
#include "externals/DirectXTex/d3dx12.h"

// c++
#include <string>
#include <unordered_map>
#include <vector>

#include <ObjectPtr.h>
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

	//! @brief デストラクタ
	~Texture() { Unload(); }

	//! @brief テクスチャのロード
	//! 
	//! @param[in] filePath ファイルパス
	void Load(const std::string& filePath, DirectXCommon* dxCommon);
	
	//! @brief テクスチャの解放
	void Unload();

	//! @brief テクスチャのGPUハンドルを取得
	//! 
	//! @return テクスチャのGPUハンドルを返却
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetHandle() const { return handleGPU_; }

private:

	ComPtr<ID3D12Resource>      textureResource_;
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU_;
	uint32_t                    descriptorIndex_;

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
	TextureManager();

	//! @brief デストラクタ
	~TextureManager();

	//! @brief 初期化処理
	void Init(DirectXCommon* dxCommon);

	//! @brief 終了処理
	void Term();

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetHandleGPU(const std::string& key) const {
		auto it = textures_.find(key);
		assert(it != textures_.end());

		return it->second.texture->GetHandle();
	}

	void LoadTexture(const std::string& filePath);

	void UnloadTexture(const std::string& filePath);

	static TextureManager* GetInstance();

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// TextureData structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct TextureData {
		Object_Ptr<Texture> texture;
		uint32_t referenceNum = 0;
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unordered_map<std::string, TextureData> textures_;
	//!< key = filePath, value = texture

	DirectXCommon* dxCommon_;
};

////////////////////////////////////////////////////////////////////////////////////////////
// TextureMethod namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace TextureMethod {

	DirectX::ScratchImage LoadTexture(const std::string& filePath);

	ID3D12Resource* CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata);

	void UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImage);

}