#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* ucontent
#include "UBaseContent.h"
#include "UContentStorage.h"

//* c++
#include <memory>
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////
// ContentCondition enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class UContentCondition : uint8_t {
	Unregistered, //!< 未登録
	Expired,      //!< 期限切れ
	Valid         //!< 有効
};


////////////////////////////////////////////////////////////////////////////////////////////
// UContentObserver class
////////////////////////////////////////////////////////////////////////////////////////////
template <UContentConcept T>
class UContentObserver {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Reset();

	void Register(const std::shared_ptr<T>& content);

	void Reload();

	UContentCondition GetCondition() const;

	//* operator *//

	void operator=(const std::shared_ptr<T>& content) { Register(content); }

	//* getter *//

	bool IsExpired() const { return GetCondition() == UContentCondition::Expired; }

	std::shared_ptr<T> Acquire();
	std::shared_ptr<T> WaitAcquire();

	std::shared_ptr<T> Get() const;
	std::shared_ptr<T> WaitGet() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::optional<std::weak_ptr<T>> content_; //!< 監視対象のcontent

	//* parameter *//

	std::filesystem::path filepath_; //!< content filepath.
	std::any param_;                 //!< content parameter.

};

////////////////////////////////////////////////////////////////////////////////////////////
// UContentObserver class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <UContentConcept T>
inline void UContentObserver<T>::Reset() {
	content_ = std::nullopt;
	filepath_.clear();
	param_ = {};
}

template <UContentConcept T>
inline void UContentObserver<T>::Register(const std::shared_ptr<T>& content) {
	content_    = content;
	filepath_   = content->GetFilepath();
	param_      = content->GetParam();
}

template <UContentConcept T>
inline void UContentObserver<T>::Reload() {
	UContentCondition condition = GetCondition();
	Exception::Assert(condition != UContentCondition::Unregistered, "asset is not registered.");

	Register(sUContentStorage->Import<T>(filepath_, param_));
}

template <UContentConcept T>
inline UContentCondition UContentObserver<T>::GetCondition() const {
	if (!content_.has_value()) {
		return UContentCondition::Unregistered;
	}

	if ((*content_).expired()) {
		return UContentCondition::Expired;
	}

	return UContentCondition::Valid;
}

template <UContentConcept T>
inline std::shared_ptr<T> UContentObserver<T>::Acquire() {
	UContentCondition condition = GetCondition();
	Exception::Assert(condition != UContentCondition::Unregistered, "asset is not registered.");

	if (condition == UContentCondition::Expired) {
		Register(sUContentStorage->Import<T>(filepath_, param_));
	}

	return (*content_).lock();
}

template <UContentConcept T>
inline std::shared_ptr<T> UContentObserver<T>::WaitAcquire() {
	std::shared_ptr<T> content = Acquire();
	content->WaitComplete();
	return content;
}

template <UContentConcept T>
inline std::shared_ptr<T> UContentObserver<T>::Get() const {
	UContentCondition condition = GetCondition();
	Exception::Assert(condition != UContentCondition::Unregistered, "asset is not registered.");
	Exception::Assert(condition != UContentCondition::Expired,      "asset is expired.");

	return (*content_).lock();
}

template<UContentConcept T>
inline std::shared_ptr<T> UContentObserver<T>::WaitGet() const {
	std::shared_ptr<T> content = Get();
	content->WaitComplete();
	return content;
}
