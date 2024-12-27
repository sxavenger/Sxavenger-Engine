#include "ModelAnimator.h"

////////////////////////////////////////////////////////////////////////////////////////////
// ModelAnimator class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ModelAnimator::Load(const DirectXThreadContext* context) {
	BaseAsset::CheckAndBeginLoad();

	if (modelAnimator_.first != nullptr) {
		modelAnimator_.first->Load(context);
	}

	if (modelAnimator_.second != nullptr) {
		modelAnimator_.second->Load(context);
	}

	BaseAsset::EndLoad();
}

void ModelAnimator::CreateModel() {
	modelAnimator_.first = std::make_shared<Model>();
	modelAnimator_.first->SetFilepath(filepath_);
	modelAnimator_.first->SetCollection(collection_);
}

void ModelAnimator::TryCreateModel() {
	if (modelAnimator_.first == nullptr) {
		CreateModel();
	}
}

void ModelAnimator::CreateAnimator() {
	modelAnimator_.second = std::make_shared<Animator>();
	modelAnimator_.second->SetFilepath(filepath_);
	modelAnimator_.second->SetCollection(collection_);
}

void ModelAnimator::TryCreateAnimator() {
	if (modelAnimator_.second == nullptr) {
		CreateAnimator();
	}
}

AssetObserver<Model> ModelAnimator::GetModel() const {
	Assert(modelAnimator_.first != nullptr, "model is not create.");
	AssetObserver<Model> observer = {};
	observer.Create(modelAnimator_.first, filepath_, collection_);
	return observer;
}

AssetObserver<Animator> ModelAnimator::GetAnimator() const {
	Assert(modelAnimator_.second != nullptr, "animator is not create.");
	AssetObserver<Animator> observer = {};
	observer.Create(modelAnimator_.second, filepath_, collection_);
	return observer;
}

