#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* content
#include "Texture/TextureCollection.h"

////////////////////////////////////////////////////////////////////////////////////////////
// SxavengerContent class
////////////////////////////////////////////////////////////////////////////////////////////
class SxavengerContent {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static void Init();

	static void Term();

	//-----------------------------------------------------------------------------------------
	// TextureCollection option
	//-----------------------------------------------------------------------------------------

	static std::shared_ptr<BaseTexture> TryLoadTextureSafely(const std::filesystem::path& filename, const DirectXThreadContext* context);
	static std::shared_ptr<Texture> TryLoadTexture(const std::filesystem::path& filename, const DirectXThreadContext* context);

	static std::shared_ptr<BaseTexture> TryCreateRenderTextureSafely(const std::string& key, const Vector2ui& size, const Color4f& clearColor = kDefaultTextureClearColor, DXGI_FORMAT format = DxObject::kOffscreenFormat);
	static std::shared_ptr<RenderTexture> TryCreateRenderTexture(const std::string& key, const Vector2ui& size, const Color4f& clearColor = kDefaultTextureClearColor, DXGI_FORMAT format = DxObject::kOffscreenFormat);

	static std::shared_ptr<BaseTexture> TryCreateUnorderedTextureSafely(const std::string& key, const Vector2ui& size, DXGI_FORMAT format = DxObject::kOffscreenFormat);
	static std::shared_ptr<UnorderedTexture> TryCreateUnorderedTexture(const std::string& key, const Vector2ui& size, DXGI_FORMAT format = DxObject::kOffscreenFormat);

	static const D3D12_GPU_DESCRIPTOR_HANDLE& GetTextureGPUHandleSRV(const std::string& key);

private:

};