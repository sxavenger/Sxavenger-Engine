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

	// デバイス, CommandListの取り出し
	ID3D12Device* device = dxCommon_->GetDeviceObj()->GetDevice();
	ID3D12GraphicsCommandList* commandList = dxCommon->GetCommandList();

	DirectX::ScratchImage mipImage = TextureMethod::LoadTexture(filePath);
	const DirectX::TexMetadata metadata = mipImage.GetMetadata();

	textureResource_ = TextureMethod::CreateTextureResource(device, metadata);
	ComPtr<ID3D12Resource> intermediateResouce = TextureMethod::UploadTextureData(textureResource_.Get(), mipImage, device, commandList);

	dxCommon->Sent();

	intermediateResouce->Release();

	// SRV - shaderResourceViewの生成
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format                  = metadata.format;
		desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		desc.ViewDimension           = D3D12_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipLevels     = UINT(metadata.mipLevels);

		// SRVを生成するDescriptorHeapの場所を決める
		descriptorIndex_ = dxCommon_->GetDescriptorsObj()->GetDescriptorCurrentIndex(DxObject::DescriptorType::SRV);

		// SRVの生成
		device->CreateShaderResourceView(
			textureResource_.Get(),
			&desc,
			dxCommon_->GetDescriptorsObj()->GetCPUDescriptorHandle(DxObject::DescriptorType::SRV, descriptorIndex_)
		);

		handleGPU_ = dxCommon_->GetDescriptorsObj()->GetGPUDescriptorHandle(DxObject::DescriptorType::SRV, descriptorIndex_);
	}
}

void Texture::Unload() {
	textureResource_->Release();
	dxCommon_->GetDescriptorsObj()->Erase(DxObject::DescriptorType::SRV, descriptorIndex_);
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
	textures_["resources/model/uvChecker.png"];
	textures_["resources/model/monsterBall.png"];
	/*textures_["resources/model/checkerBoard.png"];*/

	/*textures_["resources/particleDemo.png"];
	textures_["resources/model/wireFrame.png"];*/

	for (auto& pair : textures_) {
		pair.second.texture = std::make_unique<Texture>(pair.first, dxCommon_);
		pair.second.referenceNum = 1;
	}
}

void TextureManager::Term() {

	for (auto& pair : textures_) {
		pair.second.texture.reset();
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
	textures_[filePath].texture = std::make_unique<Texture>(filePath, dxCommon_);
	textures_[filePath].referenceNum = 1;
}

void TextureManager::UnloadTexture(const std::string& filePath) {
	auto it = textures_.find(filePath);
	assert(it != textures_.end()); //!< 同一keyが見つからなかった

	// 参照先が消える
	textures_[filePath].referenceNum--;

	if (textures_[filePath].referenceNum == 0) { //!< 参照先がない場合
		textures_[filePath].texture.reset();
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
	prop.Type                 = D3D12_HEAP_TYPE_DEFAULT;

	// Resourceの生成
	ID3D12Resource* result = nullptr;

	auto hr = device->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&result)
	);

	assert(SUCCEEDED(hr));

	return result;
}

[[nodiscard]]
ComPtr<ID3D12Resource> TextureMethod::UploadTextureData(
	ID3D12Resource* texture, const DirectX::ScratchImage& mipImages,
	ID3D12Device* device, ID3D12GraphicsCommandList* commandList) {

	std::vector<D3D12_SUBRESOURCE_DATA> subresource;
	DirectX::PrepareUpload(device, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresource);

	uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subresource.size()));
	ComPtr<ID3D12Resource> intermediateResource = DxObjectMethod::CreateBufferResource(device, intermediateSize);
	UpdateSubresources(commandList, texture, intermediateResource.Get(), 0, 0, UINT(subresource.size()), subresource.data());

	// 転送後は利用できるように D3D12_RESOUCE_STATE_COPY_DESC -> D3D12_RESOUCE_STATE_GENETIC_READ へ変更
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource   = texture;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_GENERIC_READ;

	commandList->ResourceBarrier(1, &barrier);

	return intermediateResource;

	// TODO: 02-04 ex advance
}
