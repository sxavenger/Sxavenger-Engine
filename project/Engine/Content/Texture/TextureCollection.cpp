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
	TryLoadTexture("resources/white1x1.png", SxavengerSystem::GetMainThreadContext()); //!< default texture の読み込み.
}

void TextureCollection::Term() {
}

std::shared_ptr<BaseTexture> TextureCollection::TryLoadTextureSafely(const std::string& filename, DirectXThreadContext* context) {
	if (!textures_.Contains(filename)) {
		// 新しくtextureを生成
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		texture->Load(filename, context);
		textures_.Emplace(filename, std::move(texture));
	}

	return textures_.At(filename);
}

std::shared_ptr<Texture> TextureCollection::TryLoadTexture(const std::string& filename, DirectXThreadContext* context) {
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

std::shared_ptr<BaseTexture> TextureCollection::TryCreateDummyTextureSafely(const std::string& key, const Vector2ui& size, DXGI_FORMAT format) {
	if (!textures_.Contains(key)) {
		// 新しくtextureを生成
		std::shared_ptr<DummyTexture> texture = std::make_shared<DummyTexture>();
		texture->Create(size, format);
		textures_.Emplace(key, std::move(texture));
	}

	return textures_.At(key);
}

std::shared_ptr<DummyTexture> TextureCollection::TryCreateDummyTexture(const std::string& key, const Vector2ui& size, DXGI_FORMAT format) {
	return GetTexture<DummyTexture>(TryCreateDummyTextureSafely(key, size, format));
}

void TextureCollection::ReleaseTexture(const std::string& key) {
	if (!textures_.Contains(key)) {
		return; //!< keyのtextureが見つからなかった.
	}

	textures_.Erase(key);
}
