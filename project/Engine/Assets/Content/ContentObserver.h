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
//! @brief ContentObserverが監視するContentの状態
enum class ContentCondition : uint8_t {
	Unregistered, //!< 未登録
	Expired,      //!< 期限切れ
	Valid         //!< 有効
};

////////////////////////////////////////////////////////////////////////////////////////////
// ContentObserver class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief Contentをweak_ptrで監視し, 破棄済みなら再読み込みして取得する監視ハンドル
//! @tparam T 監視対象のContent型
template <Content T>
class ContentObserver {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ContentObserver() = default;
	ContentObserver(const std::shared_ptr<T>& content) { Register(content); }

	//! @brief 監視対象を解除し, 保持情報をクリアする
	void Reset();

	//! @brief 監視対象のContentを登録する
	//! @param[in] content 監視するContent
	void Register(const std::shared_ptr<T>& content);

	//! @brief 記録済みのファイルパス/パラメータから強制的に再読み込みする
	void Reload();

	//! @brief 監視対象の現在の状態を取得する
	//! @return Contentの状態
	ContentCondition GetCondition() const;

	//* operator [assign] *//

	//! @brief 監視対象のContentを登録する (代入演算子版)
	void operator=(const std::shared_ptr<T>& content) { Register(content); }

	//* getter *//

	//! @brief 監視対象が期限切れ(破棄済み)かを返す
	//! @retval true  期限切れ
	//! @retval false それ以外
	bool IsExpired() const { return GetCondition() == ContentCondition::Expired; }

	//! @brief Contentを取得する. 期限切れなら再読み込みしてから返す
	//! @return Contentのshared_ptr
	std::shared_ptr<T> Acquire();
	//! @brief Acquireした上で読み込み完了を待って返す
	//! @return Contentのshared_ptr
	std::shared_ptr<T> WaitAcquire();

	//! @brief Contentを取得する. 期限切れの場合はアサートする
	//! @return Contentのshared_ptr
	std::shared_ptr<T> Get() const;
	//! @brief Getした上で読み込み完了を待って返す
	//! @return Contentのshared_ptr
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
