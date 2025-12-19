#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* content
#include "BaseContent.h"
#include "ContentStorage.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/Utility/StreamLogger.h>

//* c++
#include <memory>
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// ContentCondition enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class ContentCondition : uint8_t {
	Unregistered, //!< 未登録
	Expired,      //!< 期限切れ
	Valid         //!< 有効
};

////////////////////////////////////////////////////////////////////////////////////////////
// UContentObserver class
////////////////////////////////////////////////////////////////////////////////////////////
template <ContentConcept T>
class ContentObserver {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Reset();

	void Register(const std::shared_ptr<T>& content);

	void Reload();

	ContentCondition GetCondition() const;

	//* operator *//

	void operator=(const std::shared_ptr<T>& content) { Register(content); }

	//* getter *//

	bool IsExpired() const { return GetCondition() == ContentCondition::Expired; }

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

template <ContentConcept T>
inline void ContentObserver<T>::Reset() {
	content_ = std::nullopt;
	filepath_.clear();
	param_ = {};
}

template <ContentConcept T>
inline void ContentObserver<T>::Register(const std::shared_ptr<T>& content) {
	content_    = content;
	filepath_   = content->GetFilepath();
	param_      = content->GetParam();
}

template <ContentConcept T>
inline void ContentObserver<T>::Reload() {
	ContentCondition condition = GetCondition();
	StreamLogger::AssertA(condition != ContentCondition::Unregistered, "asset is not registered.");

	Register(sContentStorage->Import<T>(filepath_, param_));
}

template <ContentConcept T>
inline ContentCondition ContentObserver<T>::GetCondition() const {
	if (!content_.has_value()) {
		return ContentCondition::Unregistered;
	}

	if ((*content_).expired()) {
		return ContentCondition::Expired;
	}

	return ContentCondition::Valid;
}

template <ContentConcept T>
inline std::shared_ptr<T> ContentObserver<T>::Acquire() {
	ContentCondition condition = GetCondition();
	StreamLogger::AssertA(condition != ContentCondition::Unregistered, "asset is not registered.");

	if (condition == ContentCondition::Expired) {
		Register(sContentStorage->Import<T>(filepath_, param_));
	}

	return (*content_).lock();
}

template <ContentConcept T>
inline std::shared_ptr<T> ContentObserver<T>::WaitAcquire() {
	std::shared_ptr<T> content = Acquire();
	content->WaitComplete();
	return content;
}

template <ContentConcept T>
inline std::shared_ptr<T> ContentObserver<T>::Get() const {
	ContentCondition condition = GetCondition();
	StreamLogger::AssertA(condition != ContentCondition::Unregistered, "asset is not registered.");
	StreamLogger::AssertA(condition != ContentCondition::Expired,      "asset is expired.");

	return (*content_).lock();
}

template <ContentConcept T>
inline std::shared_ptr<T> ContentObserver<T>::WaitGet() const {
	std::shared_ptr<T> content = Get();
	content->WaitComplete();
	return content;
}

SXAVENGER_ENGINE_NAMESPACE_END
