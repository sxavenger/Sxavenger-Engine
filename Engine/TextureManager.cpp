#include "TextureManager.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <Logger.h>
#include <cassert>
#include <Console.h>

//-----------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------
using namespace DxObject;

////////////////////////////////////////////////////////////////////////////////////////////
// Texture class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Texture::Load(DirectXCommon* dxCommon, const std::string& filePath) {

	// 引数の保存
	dxCommon_ = dxCommon;

	// デバイス, CommandListの取り出し
	ID3D12Device* device                   = dxCommon_->GetDeviceObj()->GetDevice();
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	DirectX::ScratchImage mipImage = TextureMethod::LoadTexture(filePath);
	const DirectX::TexMetadata metadata = mipImage.GetMetadata();

	resource_ = TextureMethod::CreateTextureResource(device, metadata);
	intermediateResouce_ = TextureMethod::UploadTextureData(resource_.Get(), mipImage, device, commandList);

	// SRV - shaderResourceViewの生成
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format                  = metadata.format;
		desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		desc.ViewDimension           = D3D12_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipLevels     = UINT(metadata.mipLevels);

		// SRVを生成するDescriptorHeapの場所を決める
		descriptorSRV_ = dxCommon_->GetDescriptorsObj()->GetCurrentDescriptor(SRV);

		// SRVの生成
		device->CreateShaderResourceView(
			resource_.Get(),
			&desc,
			descriptorSRV_.handleCPU
		);
	}
}

void Texture::Unload() {
	dxCommon_->GetDescriptorsObj()->Erase(descriptorSRV_);
}

////////////////////////////////////////////////////////////////////////////////////////////
// RenderTexture class methods
////////////////////////////////////////////////////////////////////////////////////////////

void RenderTexture::Create(
	DirectXCommon* dxCommon,
	int32_t textureWidth, int32_t textureHeight, const Vector4f& clearColor, DXGI_FORMAT format) {
	// 引数の保存
	dxCommon_   = dxCommon;
	clearColor_ = clearColor;

	// deviceの取り出し
	auto device = dxCommon_->GetDeviceObj()->GetDevice();

	// resourceの生成
	{
		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Width            = textureWidth;
		desc.Height           = textureHeight;
		desc.MipLevels        = 1;
		desc.Format           = format;
		desc.Flags            = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		desc.SampleDesc.Count = 1;
		desc.DepthOrArraySize = 1;

		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_DEFAULT;

		D3D12_CLEAR_VALUE clearValue = {};
		clearValue.Format   = format;
		clearValue.Color[0] = clearColor_.r;
		clearValue.Color[1] = clearColor_.g;
		clearValue.Color[2] = clearColor_.b;
		clearValue.Color[3] = clearColor_.a;

		device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES,
			&desc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			&clearValue,
			IID_PPV_ARGS(&resource_)
		);
	}

	// SRV - shaderResourceViewの生成
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format                  = format;
		desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		desc.ViewDimension           = D3D12_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipLevels     = 1;

		// SRVを生成するDescriptorHeapの場所を決める
		descriptorSRV_ = dxCommon_->GetDescriptorsObj()->GetCurrentDescriptor(SRV);

		// SRVの生成
		device->CreateShaderResourceView(
			resource_.Get(),
			&desc,
			descriptorSRV_.handleCPU
		);
	}

	// RTV - RenderTargetViewの生成
	{
		descriptorRTV_ = dxCommon_->GetDescriptorsObj()->GetCurrentDescriptor(RTV);

		device->CreateRenderTargetView(
			resource_.Get(),
			nullptr,
			descriptorRTV_.handleCPU
		);
	}
}

void RenderTexture::Term() {
	dxCommon_->GetDescriptorsObj()->Erase(descriptorRTV_);
}

////////////////////////////////////////////////////////////////////////////////////////////
// DummyTexture class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DummyTexture::Create(DirectXCommon* dxCommon, int32_t textureWidth, int32_t textureHeight, DXGI_FORMAT format) {
	// 引数の保存
	dxCommon_ = dxCommon;

	// deviceの取り出し
	auto device = dxCommon_->GetDeviceObj()->GetDevice();

	// resourceの生成
	{
		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_DEFAULT;

		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Width            = textureWidth;
		desc.Height           = textureHeight;
		desc.DepthOrArraySize = 1;
		desc.SampleDesc.Count = 1;
		desc.MipLevels        = 1;
		desc.Format           = format;
		desc.Flags            = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		

		device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			nullptr,
			IID_PPV_ARGS(&resource_)
		);
	}

	// SRV - shaderResourceViewの生成
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format                  = format;
		desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		desc.ViewDimension           = D3D12_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipLevels     = 1;

		// SRVを生成するDescriptorHeapの場所を決める
		descriptorSRV_ = dxCommon_->GetDescriptorsObj()->GetCurrentDescriptor(SRV);

		// SRVの生成
		device->CreateShaderResourceView(
			resource_.Get(),
			&desc,
			descriptorSRV_.handleCPU
		);
	}

	// UAV - unorderedAccessViewの生成
	{
		D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
		desc.ViewDimension      = D3D12_UAV_DIMENSION_TEXTURE2D;
		desc.Format             = format;

		// UAVDescriptorの取得
		descriptorUAV_ = dxCommon_->GetDescriptorsObj()->GetCurrentDescriptor(UAV);
		
		device->CreateUnorderedAccessView(
			resource_.Get(),
			nullptr,
			&desc,
			descriptorUAV_.handleCPU
		);
	}
}

void DummyTexture::Term() {
	dxCommon_->GetDescriptorsObj()->Erase(descriptorUAV_);
}

////////////////////////////////////////////////////////////////////////////////////////////
// TextureManager class methods
////////////////////////////////////////////////////////////////////////////////////////////

void TextureManager::Init(DirectXCommon* dxCommon) {

	// dxCommonの保存
	dxCommon_ = dxCommon;

	// 初期textureのload
	LoadTexture("resources/uvChecker.png");
	LoadTexture("resources/tile_black.png");
	LoadTexture("resources/tile_white.png");

}

void TextureManager::Term() {
	// containerの削除
	textureContainer_.clear();

	// dxCommonの参照解除
	dxCommon_ = nullptr;
}

Texture* TextureManager::LoadTexture(const std::string& filePath) {
	if (FindKey(filePath)) { //!< 同一keyが存在する場合
		// すでにあるtextureの返却
		textureContainer_.at(filePath).refernceCount++;
		return textureContainer_.at(filePath).texture.get();
	}

	// textureの生成
	std::unique_ptr<Texture> newTexture = std::make_unique<Texture>();
	newTexture->Load(dxCommon_, filePath);

	// containerに代入
	textureContainer_.emplace(filePath, std::move(newTexture));
	return textureContainer_[filePath].texture.get();
}

Texture* TextureManager::CreateRenderTexture(const std::string& key, int32_t textureWidth, int32_t textureHeight, const Vector4f& clearColor) {
	if (FindKey(key)) { //!< 同一keyが存在する場合
		// すでにあるtextureの返却
		textureContainer_.at(key).refernceCount++;
		return textureContainer_.at(key).texture.get();
	}

	// renderTextureの生成
	std::unique_ptr<RenderTexture> newRenderTexture = std::make_unique<RenderTexture>();
	newRenderTexture->Create(dxCommon_, textureWidth, textureHeight, clearColor);

	// containerに代入
	textureContainer_.emplace(key, std::move(newRenderTexture));
	return textureContainer_[key].texture.get();
}

Texture* TextureManager::CreateDummyTexture(const std::string& key, int32_t textureWidth, int32_t textureHeight) {
	if (FindKey(key)) { //!< 同一keyが存在する場合
		// すでにあるtextureの返却
		textureContainer_.at(key).refernceCount++;
		return textureContainer_.at(key).texture.get();
	}

	// renderTextureの生成
	std::unique_ptr<DummyTexture> newDummyTexture = std::make_unique<DummyTexture>();
	newDummyTexture->Create(dxCommon_, textureWidth, textureHeight);

	// containerに代入
	textureContainer_.emplace(key, std::move(newDummyTexture));
	return textureContainer_[key].texture.get();
}

Texture* TextureManager::GetTexture(const std::string& key) const {
	assert(FindKey(key)); //!< 同一keyが見つからない
	
	return textureContainer_.at(key).texture.get();
}

const D3D12_GPU_DESCRIPTOR_HANDLE& TextureManager::GetGPUHandle(const std::string& key) const {
	assert(FindKey(key));

	return textureContainer_.at(key).texture->GetGPUHandleSRV();
}

void TextureManager::ReleaseTexture(const std::string& key) {
	if (!FindKey(key)) { //!< keyが見つからなかった場合

		std::string text = "[TextureManager](method: ReleaseTexture): not found key. key: " + key;
		console->SetLog(
			text,
			Console::warningColor
		);

		return;
	}

	// keyの参照カウントを減らす
	textureContainer_.at(key).refernceCount--;

	if (textureContainer_.at(key).refernceCount == 0) { //!< 参照カウントが0の場合
		textureContainer_.erase(key); //!< containerからの削除

		// logで通知
		std::string text = "[TextureManager](method: ReleaseTexture): delete texture. key: " + key;
		console->SetLog(
			text,
			Console::commentOutColor
		);
	}
}

void TextureManager::DeleteTexture(const std::string& key) {
	if (!FindKey(key)) { //!< keyが見つからなかった場合

		// logで通知
		console->SetLog(
			"[TextureManager](method: DeleteTexture): not found key. key: " + key,
			Console::warningColor
		);

		return;
	}

	textureContainer_.erase(key); //!< containerからの削除

	// logで通知
	console->SetLog(
		"[TextureManager](method: DeleteTexture): delete texture. key: " + key,
		Console::commentOutColor
	);

}

TextureManager* TextureManager::GetInstance() {
	static TextureManager instance;
	return &instance;
}

//=========================================================================================
// private methods
//=========================================================================================

bool TextureManager::FindKey(const std::string& key) const {
	auto it = textureContainer_.find(key);
	if (it == textureContainer_.end()) {
		return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
// TextureMethod namespace methods
////////////////////////////////////////////////////////////////////////////////////////////

DirectX::ScratchImage TextureMethod::LoadTexture(const std::string& filePath) {
	DirectX::ScratchImage image = {};
	std::wstring filePathW = ToWstring(filePath); //!< wstringに変換

	auto hr = DirectX::LoadFromWICFile(
		filePathW.c_str(),
		DirectX::WIC_FLAGS_FORCE_SRGB,
		nullptr,
		image
	);

	if (FAILED(hr)) {
		std::string errorLog;
		errorLog
			= "[Texture Not Found] \n filePath: " + filePath;

		ExternalLogger::Write(errorLog);
		Assert(false, errorLog, "Error: LoadTexture");
	}

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

ComPtr<ID3D12Resource> TextureMethod::CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata) {
	// デスクの設定
	D3D12_RESOURCE_DESC desc = {};
	desc.Width = UINT(metadata.width);
	desc.Height = UINT(metadata.height);
	desc.MipLevels = UINT16(metadata.mipLevels);
	desc.DepthOrArraySize = UINT16(metadata.arraySize);
	desc.Format = metadata.format;
	desc.SampleDesc.Count = 1;
	desc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);

	// ヒーププロパティの設定
	D3D12_HEAP_PROPERTIES prop = {};
	prop.Type = D3D12_HEAP_TYPE_DEFAULT;

	// Resourceの生成
	ComPtr<ID3D12Resource> result;

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
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = texture;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;

	commandList->ResourceBarrier(1, &barrier);

	return intermediateResource;
}
