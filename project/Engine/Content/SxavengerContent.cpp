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
	std::unique_ptr<TextureCollection> sTextureCollection = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////
// SxavengerContent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SxavengerContent::Init() {
	sTextureCollection = std::make_unique<TextureCollection>();
	sTextureCollection->Init();
}

void SxavengerContent::Term() {
	sTextureCollection.reset();
}

std::shared_ptr<Texture> SxavengerContent::TryLoadTexture(const std::string& filename, const DirectXThreadContext* context) {
	return sTextureCollection->TryLoadTexture(filename, context);
}
