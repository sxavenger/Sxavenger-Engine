#include "Texture.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Console/SystemConsole.h>

////////////////////////////////////////////////////////////////////////////////////////////
// BaseTexture base class
////////////////////////////////////////////////////////////////////////////////////////////

void BaseTexture::Term() {
	descriptorHepas_->DeleteDescriptor(descriptorSRV_);
}

////////////////////////////////////////////////////////////////////////////////////////////
// Texture class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Texture::Load(DirectXCommon* dxCommon, const std::string& filePath) {

	// 引数の保存
	descriptorHepas_ = dxCommon->GetDescriptorsObj();

	// デバイス, CommandListの取り出し
	ID3D12Device* device                   = dxCommon->GetDeviceObj()->GetDevice();
	ID3D12GraphicsCommandList* commandList = dxCommon->GetCommandList();

	DirectX::ScratchImage mipImage = LoadTexture(filePath);
	const DirectX::TexMetadata metadata = mipImage.GetMetadata();

	resource_ = CreateTextureResource(device, metadata);
	intermediateResouce_ = UploadTextureData(resource_.Get(), mipImage, device, commandList);

	// SRV - shaderResourceViewの生成
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format                  = metadata.format;
		desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		if (metadata.IsCubemap()) { //!< mipImageがcubeMapの場合
			desc.ViewDimension                   = D3D12_SRV_DIMENSION_TEXTURECUBE;
			desc.TextureCube.MostDetailedMip     = 0;
			desc.TextureCube.MipLevels           = UINT_MAX;
			desc.TextureCube.ResourceMinLODClamp = 0.0f;

		} else {
			// それ以外はTexture2D扱い
			desc.ViewDimension       = D3D12_SRV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MipLevels = UINT(metadata.mipLevels);
		}

		// SRVを生成するDescriptorHeapの場所を決める
		descriptorSRV_ = descriptorHepas_->GetDescriptor(SRV);

		// SRVの生成
		device->CreateShaderResourceView(
			resource_.Get(),
			&desc,
			descriptorSRV_.GetCPUHandle()
		);
	}

	size_ = { static_cast<uint32_t>(metadata.width), static_cast<uint32_t>(metadata.height) };
}

DirectX::ScratchImage Texture::LoadTexture(const std::string& filePath) {
	DirectX::ScratchImage image = {};
	std::wstring filePathW = ToWstring(filePath); //!< wstringに変換

	HRESULT hr;

	if (filePathW.ends_with(L".dds")) { //!< .ddsでpathが終わっている場合
		// DDSファイルとして読み込み
		hr = DirectX::LoadFromDDSFile(
			filePathW.c_str(),
			DirectX::DDS_FLAGS_NONE,
			nullptr,
			image
		);

	} else {
		hr = DirectX::LoadFromWICFile(
			filePathW.c_str(),
			DirectX::WIC_FLAGS_FORCE_SRGB,
			nullptr,
			image
		);
	}

	if (FAILED(hr)) {
		std::string errorLog;
		errorLog
			= "Texture not found. \n filePath: " + filePath;

		ExternalLogger::Write(errorLog);
		Assert(false, errorLog);
	}

	// MipMapsの生成
	DirectX::ScratchImage mipImage = {};

	if (DirectX::IsCompressed(image.GetMetadata().format)) { //!< 圧縮formatかどうか調べる
		// 圧縮formatならそのまま使う
		mipImage = std::move(image);

	} else {

		auto metadata = image.GetMetadata();

		if (metadata.width == 1 && metadata.height == 1) {

			//!< 1x1ならMipMapの生成をスキップ
			mipImage = std::move(image);

		} else {

			hr = DirectX::GenerateMipMaps(
				image.GetImages(),
				image.GetImageCount(),
				image.GetMetadata(),
				DirectX::TEX_FILTER_SRGB,
				0,
				mipImage
			);

			Assert(SUCCEEDED(hr));
		}
	}

	return mipImage;
}

ComPtr<ID3D12Resource> Texture::CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata) {
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

	Assert(SUCCEEDED(hr));

	return result;
}

ComPtr<ID3D12Resource> Texture::UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages, ID3D12Device* device, ID3D12GraphicsCommandList* commandList) {

	std::vector<D3D12_SUBRESOURCE_DATA> subresource;
	DirectX::PrepareUpload(device, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresource);

	uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subresource.size()));
	ComPtr<ID3D12Resource> intermediateResource = CreateBufferResource(device, intermediateSize);
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
}

////////////////////////////////////////////////////////////////////////////////////////////
// RenderTexture class methods
////////////////////////////////////////////////////////////////////////////////////////////

void RenderTexture::Create(DirectXCommon* dxCommon, const Vector2ui& size, const Color4f& clearColor, DXGI_FORMAT format) {

	// 引数の保存
	descriptorHepas_ = dxCommon->GetDescriptorsObj();
	clearColor_      = clearColor;
	format_          = format;
	size_            = size;

	// deviceの取り出し
	auto device = dxCommon->GetDeviceObj()->GetDevice();

	// resourceの生成
	{
		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Width            = size.x;
		desc.Height           = size.y;
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
		descriptorSRV_ = descriptorHepas_->GetDescriptor(CBV_SRV_UAV);

		// SRVの生成
		device->CreateShaderResourceView(
			resource_.Get(),
			&desc,
			descriptorSRV_.GetCPUHandle()
		);
	}

	// RTV - RenderTargetViewの生成
	{
		descriptorRTV_ = descriptorHepas_->GetDescriptor(RTV);

		device->CreateRenderTargetView(
			resource_.Get(),
			nullptr,
			descriptorRTV_.GetCPUHandle()
		);
	}

}

void RenderTexture::Term() {
	descriptorHepas_->DeleteDescriptor(descriptorRTV_);
}

////////////////////////////////////////////////////////////////////////////////////////////
// DummyTexture class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DummyTexture::Create(DirectXCommon* dxCommon, const Vector2ui& size, DXGI_FORMAT format) {

	// 引数の保存
	descriptorHepas_ = dxCommon->GetDescriptorsObj();
	format_          = format;
	size_            = size;

	// deviceの取り出し
	auto device = dxCommon->GetDeviceObj()->GetDevice();

	// resourceの生成
	{
		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_DEFAULT;

		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Width            = size.x;
		desc.Height           = size.y;
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
		descriptorSRV_ = dxCommon->GetDescriptorsObj()->GetDescriptor(SRV);

		// SRVの生成
		device->CreateShaderResourceView(
			resource_.Get(),
			&desc,
			descriptorSRV_.GetCPUHandle()
		);
	}

	// UAV - unorderedAccessViewの生成
	{
		D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
		desc.ViewDimension      = D3D12_UAV_DIMENSION_TEXTURE2D;
		desc.Format             = format;

		// UAVDescriptorの取得
		descriptorUAV_ = dxCommon->GetDescriptorsObj()->GetDescriptor(UAV);
		
		device->CreateUnorderedAccessView(
			resource_.Get(),
			nullptr,
			&desc,
			descriptorUAV_.GetCPUHandle()
		);
	}
}

void DummyTexture::Term() {
	descriptorHepas_->DeleteDescriptor(descriptorUAV_);
}

////////////////////////////////////////////////////////////////////////////////////////////
// TextureManager class methods
////////////////////////////////////////////////////////////////////////////////////////////

const std::string TextureManager::directoryPath_ = "./resources/";

void TextureManager::Init(DirectXCommon* dxCommon) {

	// dxCommonの保存
	dxCommon_ = dxCommon;

	// 初期textureのload
	/*LoadTexture("resources/uvChecker.png");
	LoadTexture("resources/tile_black.png");
	LoadTexture("resources/tile_white.png");*/

	LoadTexture("resources/white1x1.png");
}

void TextureManager::Term() {
	// containerの削除
	textures_.clear();

	// dxCommonの参照解除
	dxCommon_ = nullptr;
}

Texture* TextureManager::LoadTexture(const std::string& filePath) {

	std::string lower = ToLower(filePath);

	if (FindKey(lower)) { //!< 同一keyが存在する場合
		// 参照カウントを増やす
		textures_.at(lower).refernceCount++;
		
	} else {

		// textureの生成
		std::unique_ptr<Texture> newTexture = std::make_unique<Texture>();
		newTexture->Load(dxCommon_, filePath);

		// containerに代入
		textures_.emplace(lower, std::move(newTexture));
	}
	
	return ToDerivedTexture<Texture>(textures_.at(lower).texture.get());
}

RenderTexture* TextureManager::CreateRenderTexture(const std::string& key, const Vector2ui& textureSize, const Color4f& clearColor) {
	
	std::string lower = ToLower(key);

	if (FindKey(lower)) { //!< 同一keyが存在する場合
		// 参照カウントを増やす
		textures_.at(lower).refernceCount++;

	} else {
		// renderTextureの生成
		std::unique_ptr<RenderTexture> newRenderTexture = std::make_unique<RenderTexture>();
		newRenderTexture->Create(dxCommon_, textureSize, clearColor);

		// containerに代入
		textures_.emplace(lower, std::move(newRenderTexture));
	}

	return ToDerivedTexture<RenderTexture>(textures_.at(lower).texture.get());
}

DummyTexture* TextureManager::CreateDummyTexture(const std::string& key, const Vector2ui& textureSize) {

	std::string lower = ToLower(key);

	if (FindKey(lower)) { //!< 同一keyが存在する場合
		// 参照カウントを増やす
		textures_.at(lower).refernceCount++;

	} else {

		// renderTextureの生成
		std::unique_ptr<DummyTexture> newDummyTexture = std::make_unique<DummyTexture>();
		newDummyTexture->Create(dxCommon_, textureSize);

		// containerに代入
		textures_.emplace(lower, std::move(newDummyTexture));
	}

	return ToDerivedTexture<DummyTexture>(textures_.at(lower).texture.get());
}

BaseTexture* TextureManager::GetBaseTexture(const std::string& key) {

	std::string lower = ToLower(key);

	Assert(FindKey(lower)); //!< 同一keyが見つからない

	return textures_.at(lower).texture.get();
}

const D3D12_GPU_DESCRIPTOR_HANDLE& TextureManager::GetGPUHandleSRV(const std::string& key) const {

	std::string lower = ToLower(key);

	Assert(FindKey(lower), "Not found to texture contaienr.");

	return textures_.at(lower).texture->GetGPUHandleSRV();
}

void TextureManager::ReleaseTexture(const std::string& key) {

	std::string lower = ToLower(key);

	if (!FindKey(lower)) { //!< keyが見つからなかった場合

		std::string text = "[TextureManager](method: ReleaseTexture): not found key. key: " + lower;
		sSystemConsole->Log(
			text,
			SystemConsole::kWarningColor
		);

		return;
	}

	// keyの参照カウントを減らす
	textures_.at(lower).refernceCount--;

	if (textures_.at(lower).refernceCount == 0) { //!< 参照カウントが0の場合
		textures_.erase(lower); //!< containerからの削除

		// logで通知
		std::string text = "[TextureManager](method: ReleaseTexture): delete texture. key: " + key;
		sSystemConsole->Log(
			text,
			SystemConsole::kCommentOutColor
		);
	}
}

void TextureManager::DeleteTexture(const std::string& key) {

	std::string lower = ToLower(key);

	if (!FindKey(lower)) { //!< keyが見つからなかった場合

		// logで通知
		sSystemConsole->Log(
			"[TextureManager](method: DeleteTexture): not found key. key: " + lower,
			SystemConsole::kWarningColor
		);

		return;
	}

	textures_.erase(lower); //!< containerからの削除

	// logで通知
	sSystemConsole->Log(
		"[TextureManager](method: DeleteTexture): delete texture. key: " + lower,
		SystemConsole::kCommentOutColor
	);

}

std::string TextureManager::ToLower(const std::string& str) {
	std::string result;
	result.resize(str.size());

	std::transform(str.begin(), str.end(), result.begin(), [](unsigned char c) {
		return static_cast<char>(std::tolower(c));
	});

	return result;
}

bool TextureManager::FindKey(const std::string& key) const {

	auto it = textures_.find(key);
	if (it == textures_.end()) {
		return false;
	}

	return true;
}
