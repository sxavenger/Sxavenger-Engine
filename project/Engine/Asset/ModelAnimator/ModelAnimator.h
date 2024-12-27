#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* asset
#include "../BaseAsset.h"
#include "../AssetObserver.h"
#include "Animator/Animator.h"
#include "Model/Model.h"

//* c++
#include <concepts>

////////////////////////////////////////////////////////////////////////////////////////////
// concept
////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
concept ModelAnimatorConcept = std::same_as<T, Model> || std::same_as<T, Animator>;

////////////////////////////////////////////////////////////////////////////////////////////
// ModelAnimator class
////////////////////////////////////////////////////////////////////////////////////////////
class ModelAnimator
	: public BaseAsset {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ModelAnimator()  = default;
	~ModelAnimator() = default;

	void Load(const DirectXThreadContext* context) override;

	template <ModelAnimatorConcept T>
	void Create();

	template <ModelAnimatorConcept T>
	void TryCreate();

	//* getter *//

	template <ModelAnimatorConcept T>
	std::shared_ptr<T> GetPtr() const;

	template <ModelAnimatorConcept T>
	AssetObserver<T> Get() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::pair<std::shared_ptr<Model>, std::shared_ptr<Animator>> modelAnimator_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateModel();
	void TryCreateModel();

	void CreateAnimator();
	void TryCreateAnimator();

	std::shared_ptr<Model> GetModelPtr() const { return modelAnimator_.first; }
	AssetObserver<Model> GetModel() const;

	std::shared_ptr<Animator> GetAnimatorPtr() const { return modelAnimator_.second; }
	AssetObserver<Animator> GetAnimator() const;

};

////////////////////////////////////////////////////////////////////////////////////////////
// ModelAnimator class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <ModelAnimatorConcept T>
inline void ModelAnimator::Create() {
	if constexpr (std::same_as<T, Model>) {
		CreateModel();
	} else if constexpr (std::same_as<T, Animator>) {
		CreateAnimator();
	}
}

template<ModelAnimatorConcept T>
inline void ModelAnimator::TryCreate() {
	if constexpr (std::same_as<T, Model>) {
		TryCreateModel();

	} else if constexpr (std::same_as<T, Animator>) {
		TryCreateAnimator();
	}
}

template<ModelAnimatorConcept T>
inline std::shared_ptr<T> ModelAnimator::GetPtr() const {
	if constexpr (std::same_as<T, Model>) {
		return GetModelPtr();

	} else if constexpr (std::same_as<T, Animator>) {
		return GetAnimatorPtr();

	} else {
		static_assert(false, "ModelAnimator::GetPtr() is not supported.");
	}
}

template<ModelAnimatorConcept T>
inline AssetObserver<T> ModelAnimator::Get() const {
	if constexpr (std::same_as<T, Model>) {
		return GetModel();

	} else if constexpr (std::same_as<T, Animator>) {
		return GetAnimator();

	} else {
		static_assert(false, "ModelAnimator::Get() is not supported.");
	}
}
