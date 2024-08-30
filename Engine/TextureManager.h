#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// defines
#include <DefineConfigs.h>

// DirectX12
#include <d3d12.h>
#include <DirectXTex.h>
#include <d3dx12.h>

// DxCommon
#include <DirectXCommon.h>

// c++
#include <string>
#include <unordered_map>
#include <deque>
#include <vector>
#include <memory>

// ComPtr
#include <ComPtr.h>

// Geometry
#include <Vector4.h>
#include <Vector2.h>

// TODO: variantで適切な型が取得できる用に改良

////////////////////////////////////////////////////////////////////////////////////////////
// Texture class
////////////////////////////////////////////////////////////////////////////////////////////
class Texture {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Texture() = default;

	~Texture() { Unload(); }

	//! @brief テクスチャの読み込み
	void Load(DirectXCommon* dxCommon, const std::string& filePath);

	//! @brief テクスチャの開放
	void Unload();

	//! @brief Resourceの返却
	ID3D12Resource* GetResource() const {
		return resource_.Get();
	}

	//! @brief GPUHandle(SRV)の返却
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleSRV() const {
		return descriptorSRV_.GetGPUHandle();
	}

	const Vector2ui& GetTextureSize() const { return textureSize_; }

	/* 継承先関数 */
	//!< 継承先で定義されるのでこのクラスで呼び出された場合, エラーとなる

	virtual const D3D12_CPU_DESCRIPTOR_HANDLE& GetCPUHandleRTV() const {
		assert(false);
		return descriptorSRV_.GetCPUHandle(); // 絶対に返さない
	}

	virtual const Color4f& GetClearColor() const {
		assert(false);
		return defaultClearColor; // 絶対に返さない
	}

	virtual const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleUAV() const {
		assert(false);
		return descriptorSRV_.GetGPUHandle(); // 絶対に返さない
	}

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	/* ptr parameter */
	DirectXCommon* dxCommon_ = nullptr;

	/* menbers */
	
	ComPtr<ID3D12Resource> resource_;
	DxObject::Descriptor   descriptorSRV_;

	//* parameters *//

	Vector2ui textureSize_;

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
	: public Texture { //!< RenderTarget texture
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	RenderTexture() = default;

	~RenderTexture() { Term(); }

	//! @brief RenderTextureの生成
	void Create(
		DirectXCommon* dxCommon,
		int32_t textureWidth, int32_t textureHeight, const Color4f& clearColor = defaultClearColor, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB
	);

	//! @brief 終了処理
	void Term();

	//! @brief CPUHandle(RTV)の返却
	const D3D12_CPU_DESCRIPTOR_HANDLE& GetCPUHandleRTV() const override {
		return descriptorRTV_.GetCPUHandle();
	}

	//! @brief クリアカラーの返却
	const Color4f& GetClearColor() const override {
		return clearColor_;
	}

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	DxObject::Descriptor descriptorRTV_;

	/* parameter */
	Color4f clearColor_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// DummyTexture class
////////////////////////////////////////////////////////////////////////////////////////////
class DummyTexture
	: public Texture { //!< UnorderedAccess texture
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	DummyTexture() = default;

	~DummyTexture() { Term(); }

	//! @brief DummyTextureの生成
	void Create(DirectXCommon* dxCommon, int32_t textureWidth, int32_t textureHeight, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);

	//! @brief 終了処理
	void Term();

	//! @brief GPUHandle(UAV)の返却
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleUAV() const override {
		return descriptorUAV_.GetGPUHandle();
	}

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	DxObject::Descriptor descriptorUAV_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// TextureManager class
////////////////////////////////////////////////////////////////////////////////////////////
class TextureManager {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	TextureManager() = default;
	~TextureManager() = default;

	//! @brief 初期化処理
	void Init(DirectXCommon* dxCommon);

	//! @brief 終了処理
	void Term();

	/* Textureの生成 */

	//! @brief テクスチャの読み込み
	Texture* LoadTexture(const std::string& filePath);

	//! @brief RenderTextureの生成
	Texture* CreateRenderTexture(const std::string& key, int32_t textureWidth, int32_t textureHeight, const Color4f& clearColor = defaultClearColor);

	//! @brief DummyTextureの生成
	Texture* CreateDummyTexture(const std::string& key, int32_t textureWidth, int32_t textureHeight);

	/* container関係 */

	//! @brief containerからTextureの取得
	Texture* GetTexture(const std::string& key) const;

	//! @brief containerからGPUHandleの取得
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandle(const std::string& key) const;

	//! @brief textureの解放
	void ReleaseTexture(const std::string& key);

	//! @brief textureの削除
	void DeleteTexture(const std::string& key);

	/* statics */

	static TextureManager* GetInstance();

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// TextureElement structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct TextureElement {
		// texture
		std::unique_ptr<Texture> texture;

		// parameter
		uint32_t refernceCount = 1;
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	/* static parameter */
	DirectXCommon* dxCommon_ = nullptr;

	/* container */
	std::unordered_map<std::string, TextureElement> textureContainer_;

	//=========================================================================================
	// private methods
	//=========================================================================================

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

	_NODISCARD ComPtr<ID3D12Resource> UploadTextureData(
		ID3D12Resource* texture, const DirectX::ScratchImage& mipImages,
		ID3D12Device* device, ID3D12GraphicsCommandList* commandList
	);

}