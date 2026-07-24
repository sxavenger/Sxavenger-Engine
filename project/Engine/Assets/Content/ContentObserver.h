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
// ContentObserver class
////////////////////////////////////////////////////////////////////////////////////////////
template <Content T>
class ContentObserver {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ContentObserver() = default;
	ContentObserver(const std::shared_ptr<T>& content) { Register(content); }

	void Reset();

	void Register(const std::shared_ptr<T>& content);

	void Reload();

	ContentCondition GetCondition() const;

	//* operator [assign] *//

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
	std::any parameter_;             //!< content parameter.

	//=========================================================================================
	// private methods
	//=========================================================================================

	static void Wait(const std::shared_ptr<BaseContent>& content) { content->WaitComplete(); }

};

////////////////////////////////////////////////////////////////////////////////////////////
// ContentObserver class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <Content T>
inline void ContentObserver<T>::Reset() {
	content_ = std::nullopt;
	filepath_.clear();
	parameter_ = {};
}

template <Content T>
inline void ContentObserver<T>::Register(const std::shared_ptr<T>& content) {
	content_    = content;
	filepath_   = content->GetFilepath();
	parameter_  = content->GetParameter();
}

template <Content T>
inline void ContentObserver<T>::Reload() {
	ContentCondition condition = GetCondition();
	StreamLogger::AssertA(condition != ContentCondition::Unregistered, "asset is not registered.");

	ContentObserver<T>::Register(sContentStorage->Import<T>(filepath_, parameter_));
}

template <Content T>
inline ContentCondition ContentObserver<T>::GetCondition() const {
	if (!content_.has_value()) {
		return ContentCondition::Unregistered;
	}

	if ((*content_).expired()) {
		return ContentCondition::Expired;
	}

	return ContentCondition::Valid;
}

template <Content T>
inline std::shared_ptr<T> ContentObserver<T>::Acquire() {
	ContentCondition condition = GetCondition();
	StreamLogger::AssertA(condition != ContentCondition::Unregistered, "asset is not registered.");

	if (condition == ContentCondition::Expired) {
		ContentObserver<T>::Register(sContentStorage->Import<T>(filepath_, parameter_));
	}

	return (*content_).lock();
}

template <Content T>
inline std::shared_ptr<T> ContentObserver<T>::WaitAcquire() {
	std::shared_ptr<T> content = Acquire();
	ContentObserver<T>::Wait(content);
	return content;
}

template <Content T>
inline std::shared_ptr<T> ContentObserver<T>::Get() const {
	ContentCondition condition = GetCondition();
	StreamLogger::AssertA(condition != ContentCondition::Unregistered, "asset is not registered.");
	StreamLogger::AssertA(condition != ContentCondition::Expired,      "asset is expired.");

	return (*content_).lock();
}

template <Content T>
inline std::shared_ptr<T> ContentObserver<T>::WaitGet() const {
	std::shared_ptr<T> content = Get();
	ContentObserver<T>::Wait(content);
	return content;
}

SXAVENGER_ENGINE_NAMESPACE_END
