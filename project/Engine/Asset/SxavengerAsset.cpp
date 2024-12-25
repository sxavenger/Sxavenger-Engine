#include "SxavengerAsset.h"

//=========================================================================================
// static variables
//=========================================================================================

std::unique_ptr<AssetCollection> SxavengerAsset::collection_;

////////////////////////////////////////////////////////////////////////////////////////////
// SxavengerAsset class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SxavengerAsset::Init() {
	collection_ = std::make_unique<AssetCollection>();
	collection_->Init();
}

void SxavengerAsset::Term() {
	collection_.reset();
}
