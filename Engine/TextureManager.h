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
#include <deque>
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
	Texture() {}

	//! @brief デストラクタ
	~Texture() { Unload(); }

	//! @brief テクスチャのロード
	void Load(const std::string& filePath, DirectXCommon* dxCommon);

	//! @brief テクスチャの開放
	void Unload();

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetSRVHandleGPU() const { return descriptorSRV_.handleGPU; }

	virtual const D3D12_CPU_DESCRIPTOR_HANDLE& GetRTVHandleCPU() const {
		assert(false); //!< RenderTextureではないのでRTVを持ってない
		return descriptorSRV_.handleCPU; // 絶対に返すな
	}

	ID3D12Resource* GetResource() const { return resource_.Get(); }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	DirectXCommon* dxCommon_ = nullptr;

	ComPtr<ID3D12Resource> resource_;
	DxObject::Descriptor descriptorSRV_;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	ComPtr<ID3D12Resource> intermediateResouce_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// RenderTexture class
////////////////////////////////////////////////////////////////////////////////////////////
class RenderTexture
	: public Texture {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief コンストラクタ
	RenderTexture() {}

	//! @brief デストラクタ
	~RenderTexture() { Unload(); }

	//! @brief ダミーテクスチャの作成
	void Create(int32_t width, int32_t height, DirectXCommon* dxCommon);

	//! @brief テクスチャの解放
	void Unload();

	const D3D12_CPU_DESCRIPTOR_HANDLE& GetRTVHandleCPU() const override {
		return descriptorRTV_.handleCPU;
	}

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	DxObject::Descriptor descriptorRTV_;

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

	//! @brief Textureが使えるようになる
	//! @brief HACK: commandAllocator[texture]を送信してから関数を呼び出すこと
	void EnableTexture();

	//! @brief テクスチャの読み込み
	void LoadTexture(const std::string& filePath);

	//! @brief ダミーテクスチャの生成
	void CreateRenderTexture(int32_t width, int32_t height, const std::string& key);

	//! @brief テクスチャの削除
	void UnloadTexture(const std::string& key);

	//! @brief テクスチャの削除 (参照カウントを無視)
	void DeleteTexture(const std::string& key);

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetHandleGPU(const std::string& key) const {
		auto it = GetKey(key);

		if (!it->second.isTextureEnabled) {
			assert(false); //!< textureがGPUに送られてない
		}

		return it->second.texture->GetSRVHandleGPU();
	}

	Texture* GetTexture(const std::string& key) const {
		auto it = GetKey(key);

		return it->second.texture.get();
	}

	static TextureManager* GetInstance();

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// TextureData enum
	////////////////////////////////////////////////////////////////////////////////////////////
	struct TextureData {
		std::unique_ptr<Texture> texture;

		// info //
		uint32_t referenceCount = 0;
		bool isTextureEnabled   = false;
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	DirectXCommon* dxCommon_ = nullptr;

	std::unordered_map<std::string, TextureData> textures_;
	//!< key = filePath, value = textureData
	
	std::deque<std::string> waitTextureQueue_; //!< commandList送信待ちtextureのコンテナ

	//=========================================================================================
	// private methods
	//=========================================================================================

	//! @brief keyの要素を取得
	std::unordered_map<std::string, TextureManager::TextureData>::const_iterator GetKey(const std::string& key) const;

	//! @brief keyが存在するかどうか
	//! 
	//! @retval true  存在する
	//! @retval false 存在しない
	bool FindKey(const std::string& key) const;


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