#include "TextureManager.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <windows.h>
#include <Logger.h>

#include <MyEngine.h>
#include <DirectXCommon.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Texture methods
////////////////////////////////////////////////////////////////////////////////////////////

void Texture::Load(const std::string& filePath, DirectXCommon* dxCommon) {

	// dxCommonの保存
	dxCommon_ = dxCommon;

	// デバイスの取り出し
	ID3D12Device* device = dxCommon_->GetDeviceObj()->GetDevice();

	DirectX::ScratchImage mipImage = TextureMethod::LoadTexture(filePath);
	const DirectX::TexMetadata metadata = mipImage.GetMetadata();

	textureResource_ = TextureMethod::CreateTextureResource(device, metadata);
	TextureMethod::UploadTextureData(textureResource_.Get(), mipImage);


	// SRV - shaderResourceViewの生成
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format                  = metadata.format;
		desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		desc.ViewDimension           = D3D12_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipLevels     = UINT(metadata.mipLevels);

		// SRVを生成するDescriptorHeapの場所を決める
		descriptorIndex_ = dxCommon_->GetDescriptorsObj()->GetDescriptorCurrentIndex(DxObject::DescriptorHeaps::SRV);

		// SRVの生成
		device->CreateShaderResourceView(
			textureResource_.Get(),
			&desc,
			dxCommon_->GetDescriptorsObj()->GetCPUDescriptorHandle(DxObject::DescriptorHeaps::SRV, descriptorIndex_)
		);

		handleGPU_ = dxCommon_->GetDescriptorsObj()->GetGPUDescriptorHandle(DxObject::DescriptorHeaps::SRV, descriptorIndex_);
	}
}

void Texture::Unload() {
	textureResource_->Release();
	dxCommon_->GetDescriptorsObj()->Erase(DxObject::DescriptorHeaps::SRV, descriptorIndex_);
}

////////////////////////////////////////////////////////////////////////////////////////////
// TextureManager methods
////////////////////////////////////////////////////////////////////////////////////////////

TextureManager::TextureManager() {
}

TextureManager::~TextureManager() {
}

void TextureManager::Init(DirectXCommon* dxCommon) {
	// dxCommonの保存
	dxCommon_ = dxCommon;

	// 初期texture
	textures_["resources/uvChecker.png"];
	textures_["resources/monsterBall.png"];

	for (auto& pair : textures_) {
		pair.second.texture = new Texture(pair.first, dxCommon_);
		pair.second.referenceNum = 1;
	}
}

void TextureManager::Term() {

	for (auto& pair : textures_) {
		pair.second.texture.Release();
		pair.second.referenceNum = NULL;
	}

	textures_.clear();
	dxCommon_ = nullptr;
}

void TextureManager::LoadTexture(const std::string& filePath) {
	auto it = textures_.find(filePath);
	if (it != textures_.end()) { //!< 同一keyが見つかった場合
		// 参照数にインクリメント
		textures_[filePath].referenceNum++;
		return;
	}

	// textureの登録
	textures_[filePath].texture = new Texture(filePath, dxCommon_);
	textures_[filePath].referenceNum = 1;
}

void TextureManager::UnloadTexture(const std::string& filePath) {
	auto it = textures_.find(filePath);
	assert(it != textures_.end()); //!< 同一keyが見つからなかった

	// 参照先が消える
	textures_[filePath].referenceNum--;

	if (textures_[filePath].referenceNum == 0) { //!< 参照先がない場合
		textures_[filePath].texture.Release();
		textures_.erase(filePath);
	}
}

//=========================================================================================
// static methods
//=========================================================================================
TextureManager* TextureManager::GetInstance() {
	static TextureManager instance;
	return &instance;
}

////////////////////////////////////////////////////////////////////////////////////////////
// TextureMethod namespace
////////////////////////////////////////////////////////////////////////////////////////////

DirectX::ScratchImage TextureMethod::LoadTexture(const std::string& filePath) {
	DirectX::ScratchImage image = {};
	std::wstring filePathW = ConvertString(filePath); //!< wstringに変換

	auto hr = DirectX::LoadFromWICFile(
		filePathW.c_str(),
		DirectX::WIC_FLAGS_FORCE_SRGB,
		nullptr,
		image
	);

	assert(SUCCEEDED(hr));

	// MipMapsの生成
	DirectX::ScratchImage mipImage = {};

	hr = DirectX::GenerateMipMaps(
		image.GetImages(),
		image.GetImageCount(),
		image.GetMetadata(),
		DirectX::TEX_FILTER_SRGB,
		0,
		mipImage
	);

	assert(SUCCEEDED(hr));

	return mipImage;
}

ID3D12Resource* TextureMethod::CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata) {
	// デスクの設定
	D3D12_RESOURCE_DESC desc = {};
	desc.Width            = UINT(metadata.width);
	desc.Height           = UINT(metadata.height);
	desc.MipLevels        = UINT16(metadata.mipLevels);
	desc.DepthOrArraySize = UINT16(metadata.arraySize);
	desc.Format           = metadata.format;
	desc.SampleDesc.Count = 1;
	desc.Dimension        = D3D12_RESOURCE_DIMENSION(metadata.dimension);

	// ヒーププロパティの設定
	D3D12_HEAP_PROPERTIES prop = {};
	prop.Type                 = D3D12_HEAP_TYPE_CUSTOM;
	prop.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	prop.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	// Resourceの生成
	ID3D12Resource* result = nullptr;

	auto hr = device->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&result)
	);

	assert(SUCCEEDED(hr));

	return result;
}

void TextureMethod::UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImage) {
	// metadataを取得
	const DirectX::TexMetadata& metadata = mipImage.GetMetadata();

	for (size_t mipLevel = 0; mipLevel < metadata.mipLevels; ++mipLevel) {
		// mipLevelを指定して各imageを取得
		const DirectX::Image* img = mipImage.GetImage(mipLevel, 0, 0);

		// Textureに転送
		auto hr = texture->WriteToSubresource(
			UINT(mipLevel),
			nullptr,
			img->pixels,
			UINT(img->rowPitch),
			UINT(img->slicePitch)
		);

		assert(SUCCEEDED(hr));
	}
}
