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
