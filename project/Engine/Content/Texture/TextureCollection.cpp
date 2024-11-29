#include "TextureCollection.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// TextureCollection class methods
////////////////////////////////////////////////////////////////////////////////////////////

void TextureCollection::Init() {
}

void TextureCollection::Term() {
}

std::shared_ptr<BaseTexture> TextureCollection::TryLoadTextureSafely(const std::filesystem::path& filename, const DirectXThreadContext* context) {
	if (!textures_.Contains(filename.generic_string())) {
		// 新しくtextureを生成
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		texture->Load(filename, context);
		textures_.Emplace(filename.generic_string(), std::move(texture));
	}

	return textures_.At(filename.generic_string());
}

std::shared_ptr<Texture> TextureCollection::TryLoadTexture(const std::filesystem::path& filename, const DirectXThreadContext* context) {
	return GetTexture<Texture>(TryLoadTextureSafely(filename, context));
}

std::shared_ptr<BaseTexture> TextureCollection::TryCreateRenderTextureSafely(const std::string& key, const Vector2ui& size, const Color4f& clearColor, DXGI_FORMAT format) {
	if (!textures_.Contains(key)) {
		// 新しくtextureを生成
		std::shared_ptr<RenderTexture> texture = std::make_shared<RenderTexture>();
		texture->Create(size, clearColor, format);
		textures_.Emplace(key, std::move(texture));
	}

	return textures_.At(key);
}

std::shared_ptr<RenderTexture> TextureCollection::TryCreateRenderTexture(const std::string& key, const Vector2ui& size, const Color4f& clearColor, DXGI_FORMAT format) {
	return GetTexture<RenderTexture>(TryCreateRenderTextureSafely(key, size, clearColor, format));
}

std::shared_ptr<BaseTexture> TextureCollection::TryCreateUnorderedTextureSafely(const std::string& key, const Vector2ui& size, DXGI_FORMAT format) {
	if (!textures_.Contains(key)) {
		// 新しくtextureを生成
		std::shared_ptr<UnorderedTexture> texture = std::make_shared<UnorderedTexture>();
		texture->Create(size, format);
		textures_.Emplace(key, std::move(texture));
	}

	return textures_.At(key);
}

std::shared_ptr<UnorderedTexture> TextureCollection::TryCreateUnorderedTexture(const std::string& key, const Vector2ui& size, DXGI_FORMAT format) {
	return GetTexture<UnorderedTexture>(TryCreateUnorderedTextureSafely(key, size, format));
}

const D3D12_GPU_DESCRIPTOR_HANDLE& TextureCollection::GetGPUHandleSRV(const std::string& key) const {
	return GetTexture<BaseTexture>(key)->GetGPUHandleSRV();
}

void TextureCollection::ReleaseTexture(const std::string& key) {
	if (!textures_.Contains(key)) {
		return; //!< keyのtextureが見つからなかった.
	}

	textures_.Erase(key);
}
