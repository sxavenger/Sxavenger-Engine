#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* defines
#include <Lib/CXXAttributeConfig.h>

//* DirectX12
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXTex.h>

//* DirectXCommon
#include <Engine/System/DirectXCommon.h>

//* c++
#include <string>
#include <unordered_map>
#include <memory>
#include <variant>

//* Geometry
#include <Lib/Geometry/Vector4.h>
#include <Lib/Geometry/Vector2.h>

////////////////////////////////////////////////////////////////////////////////////////////
// BaseTexture base class
////////////////////////////////////////////////////////////////////////////////////////////
class BaseTexture { //!< このクラス単体では使わない
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	virtual ~BaseTexture() { Term(); }

	void Term();

	//* getter *//

	ID3D12Resource* const GetResource() const { return resource_.Get(); }

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleSRV() const { return descriptorSRV_.GetGPUHandle(); }

	const Vector2ui& GetSize() const { return size_; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* externals *//

	DxObject::DescriptorHeaps* descriptorHepas_ = nullptr;
	//!< descriptorの動的削除用

	//* members *//

	ComPtr<ID3D12Resource> resource_;
	DxObject::Descriptor   descriptorSRV_;

	//* parameters *//

	Vector2ui size_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// Texture class
////////////////////////////////////////////////////////////////////////////////////////////
class Texture
	: public BaseTexture {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Texture() = default;

	void Load(DirectXCommon* dxCommon, const std::string& filepath);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* ID3D12 *//

	ComPtr<ID3D12Resource> intermediateResouce_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	static DirectX::ScratchImage LoadTexture(const std::string& filePath);

	static ComPtr<ID3D12Resource> CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata);

	static _NODISCARD ComPtr<ID3D12Resource> UploadTextureData(
		ID3D12Resource* texture, const DirectX::ScratchImage& mipImages,
		ID3D12Device* device, ID3D12GraphicsCommandList* commandList
	);

};

////////////////////////////////////////////////////////////////////////////////////////////
// RenderTexture class
////////////////////////////////////////////////////////////////////////////////////////////
class RenderTexture
	: public BaseTexture {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	RenderTexture() = default;

	~RenderTexture() { Term(); }

	void Create(
		DirectXCommon* dxCommon,
		const Vector2ui& size, const Color4f& clearColor = defaultClearColor, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB
	);

	void Term();

	//* getter *//

	const D3D12_CPU_DESCRIPTOR_HANDLE& GetCPUHandleRTV() const { return descriptorRTV_.GetCPUHandle(); }

	const Color4f& GetClearColor() const { return clearColor_; }

	const DXGI_FORMAT GetFormat() const { return format_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* descriptor *//

	DxObject::Descriptor descriptorRTV_;

	//* parameter *//

	Color4f clearColor_;
	DXGI_FORMAT format_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// DummyTexture class
////////////////////////////////////////////////////////////////////////////////////////////
class DummyTexture
	: public BaseTexture { //!< UnorderedAccess texture
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	DummyTexture() = default;

	~DummyTexture() { Term(); }

	//! @brief DummyTextureの生成
	void Create(DirectXCommon* dxCommon, const Vector2ui& size, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);

	//! @brief 終了処理
	void Term();

	//! @brief GPUHandle(UAV)の返却
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleUAV() const { return descriptorUAV_.GetGPUHandle(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* descriptor *//

	DxObject::Descriptor descriptorUAV_;

	//* parameter *//

	DXGI_FORMAT format_;

};

//=========================================================================================
// concept
//=========================================================================================

//! [concept]
//! BaseTextureを継承してるclassのみのtemplate
template <class T>
concept DerivedFormBaseTexture = std::is_base_of_v<BaseTexture, T>;

////////////////////////////////////////////////////////////////////////////////////////////
// TextureManager class
////////////////////////////////////////////////////////////////////////////////////////////
class TextureManager {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init(DirectXCommon* dxCommon);

	void Term();

	//* create and load option *//

	Texture* LoadTexture(const std::string& filePath);

	RenderTexture* CreateRenderTexture(const std::string& key, const Vector2ui& textureSize, const Color4f& clearColor = defaultClearColor);

	DummyTexture* CreateDummyTexture(const std::string& key, const Vector2ui& textureSize);

	//* container option *//

	template <DerivedFormBaseTexture T>
	T* GetTexture(const std::string& key);

	BaseTexture* GetBaseTexture(const std::string& key);

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleSRV(const std::string& key) const;

	//! @brief 参照カウントを込みでのtextureの解放
	void ReleaseTexture(const std::string& key);

	//! @brief textureの削除
	void DeleteTexture(const std::string& key);

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// TextureElement structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct TextureElement {
		std::unique_ptr<BaseTexture> texture;
		uint32_t                     refernceCount = 1;
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* external *//

	DirectXCommon* dxCommon_ = nullptr;

	//* container *//

	std::unordered_map<std::string, TextureElement> textures_;

	//* parameter *//

	static const std::string directoryPath_;
	 //!< directoryがmodelとの競合でいまはまだ使ってない

	//=========================================================================================
	// private methods
	//=========================================================================================

	static std::string ToLower(const std::string& str);
	
	template <DerivedFormBaseTexture T>
	static T* ToDerivedTexture(BaseTexture* base);

	bool FindKey(const std::string& key) const;

};

////////////////////////////////////////////////////////////////////////////////////////////
// TextureManager class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template<DerivedFormBaseTexture T>
T* TextureManager::GetTexture(const std::string& key) {
	
	std::string lower = ToLower(key);

	Assert(FindKey(lower)); //!< 同一keyが見つからない

	return ToDerivedTexture<T>(textures_.at(lower).texture.get());

}

template<DerivedFormBaseTexture T>
T* TextureManager::ToDerivedTexture(BaseTexture* base) {

	// 継承先のclassの取得
	T* result = dynamic_cast<T*>(base);

	Assert(result != nullptr); //!< 型の不一致

	return result;
}
