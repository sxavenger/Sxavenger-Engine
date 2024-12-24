#include "SxavengerContent.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace {
	//* content
	static std::unique_ptr<TextureCollection> sTextureCollection      = nullptr;
	static std::unique_ptr<SkinningComputePipeline> sSkinningPipeline = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////
// SxavengerContent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SxavengerContent::Init() {
	sTextureCollection = std::make_unique<TextureCollection>();
	sTextureCollection->Init();

	sSkinningPipeline = std::make_unique<SkinningComputePipeline>();
	sSkinningPipeline->Init();
}

void SxavengerContent::Term() {
	sTextureCollection.reset();
	sSkinningPipeline.reset();
}

std::shared_ptr<BaseTexture> SxavengerContent::TryCreateRenderTextureSafely(const std::string& key, const Vector2ui& size, const Color4f& clearColor, DXGI_FORMAT format) {
	return sTextureCollection->TryCreateRenderTextureSafely(key, size, clearColor, format);
}

std::shared_ptr<RenderTexture> SxavengerContent::TryCreateRenderTexture(const std::string& key, const Vector2ui& size, const Color4f& clearColor, DXGI_FORMAT format) {
	return sTextureCollection->TryCreateRenderTexture(key, size, clearColor, format);
}

std::shared_ptr<BaseTexture> SxavengerContent::TryCreateUnorderedTextureSafely(const std::string& key, const Vector2ui& size, DXGI_FORMAT format) {
	return sTextureCollection->TryCreateUnorderedTextureSafely(key, size, format);
}

std::shared_ptr<UnorderedTexture> SxavengerContent::TryCreateUnorderedTexture(const std::string& key, const Vector2ui& size, DXGI_FORMAT format) {
	return sTextureCollection->TryCreateUnorderedTexture(key, size, format);
}

const D3D12_GPU_DESCRIPTOR_HANDLE& SxavengerContent::GetTextureGPUHandleSRV(const std::string& key) {
	return sTextureCollection->GetGPUHandleSRV(key);
}

void SxavengerContent::SetSkinningPipeline(const DirectXThreadContext* context) {
	sSkinningPipeline->SetPipeline(context);
}

void SxavengerContent::DispatchSkinning(const DirectXThreadContext* context, const DxObject::BindBufferDesc& desc, uint32_t vertexSize) {
	sSkinningPipeline->Dispatch(context, desc, vertexSize);
}
