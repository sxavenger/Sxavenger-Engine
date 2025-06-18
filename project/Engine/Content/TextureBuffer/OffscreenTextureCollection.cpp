#include "OffscreenTextureCollection.h"

////////////////////////////////////////////////////////////////////////////////////////////
// OffscreenTextureCollection class methods
////////////////////////////////////////////////////////////////////////////////////////////

void OffscreenTextureCollection::RegisterTexture(const std::string& name, std::unique_ptr<BaseOffscreenTexture>&& texture) {
	textures_[name] = std::move(texture);
}

const D3D12_GPU_DESCRIPTOR_HANDLE& OffscreenTextureCollection::GetGPUHandleSRV(const std::string& name) const {
	Exception::Assert(textures_.contains(name), "texture is not found. name: " + name);
	return textures_.at(name)->GetGPUHandleSRV();
}
