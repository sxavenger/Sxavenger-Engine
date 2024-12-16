#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* content
#include "Texture/TextureCollection.h"
#include "Animation/SkinningPipeline.h"

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

	static std::shared_ptr<BaseTexture> TryCreateRenderTextureSafely(const std::string& key, const Vector2ui& size, const Color4f& clearColor = kDefaultTextureClearColor, DXGI_FORMAT format = DxObject::kOffscreenFormat);
	static std::shared_ptr<RenderTexture> TryCreateRenderTexture(const std::string& key, const Vector2ui& size, const Color4f& clearColor = kDefaultTextureClearColor, DXGI_FORMAT format = DxObject::kOffscreenFormat);

	static std::shared_ptr<BaseTexture> TryCreateUnorderedTextureSafely(const std::string& key, const Vector2ui& size, DXGI_FORMAT format = DxObject::kOffscreenFormat);
	static std::shared_ptr<UnorderedTexture> TryCreateUnorderedTexture(const std::string& key, const Vector2ui& size, DXGI_FORMAT format = DxObject::kOffscreenFormat);

	static const D3D12_GPU_DESCRIPTOR_HANDLE& GetTextureGPUHandleSRV(const std::string& key);

	//-----------------------------------------------------------------------------------------
	// Skinning option
	//-----------------------------------------------------------------------------------------

	static void SetSkinningPipeline(const DirectXThreadContext* context);

	static void DispatchSkinning(const DirectXThreadContext* context, const DxObject::BindBufferDesc& desc, uint32_t vertexSize);

private:

};