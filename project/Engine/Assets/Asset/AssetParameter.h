#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* uasset
#include "BaseAsset.h"
#include "AssetStorage.h"

//* engine
#include <Engine/System/Utility/StreamLogger.h>
#include <Engine/System/Utility/RuntimeLogger.h>

//* lib
#include <Lib/Adapter/Uuid/Uuid.h>
#include <Lib/Adapter/Json/JsonHandler.h>

//* c++
#include <variant>
#include <memory>
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// AssetState class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief AssetParameterが現在保持している値の種別
enum class AssetState : uint8_t {
	Monostate, //!< monostate:          空の状態
	Uuid,      //!< Uuid:               uassetのid
	Ptr,       //!< std::shared_ptr<T>: userが作成したuasset
};

////////////////////////////////////////////////////////////////////////////////////////////
// AssetParameter class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief Assetをuuid参照または直接shared_ptrのいずれかで保持する汎用パラメータ
//! @tparam T 対象のAsset型
template <Asset T>
class AssetParameter {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Parameter variant
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief 空/uuid参照/直接ポインタの3状態を表すvariant
	using Parameter = std::variant<std::monostate, Uuid, std::shared_ptr<T>>;
	//! monostate: 空の状態
	//! Uuid: uassetのid
	//! std::shared_ptr<T>: userが作成したuasset

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	AssetParameter() = default;
	AssetParameter(const std::nullptr_t) { Reset(); }
	AssetParameter(const std::shared_ptr<T>& asset) { Set(asset); }
	AssetParameter(const Uuid& id) { Set(id); }

	//! @brief 現在保持している値の種別を取得する
	//! @return 保持状態
	AssetState GetState() const;

	//! @brief 値が未設定かを返す
	//! @retval true  空
	//! @retval false 値を保持している
	bool Empty() const { return GetState() == AssetState::Monostate; }

	//! @brief 保持している値を空にする
	void Reset() { parameter_ = std::monostate{}; }

	//! @brief 直接shared_ptrでAssetを設定する
	//! @param[in] asset 設定するAsset
	void Set(const std::shared_ptr<T>& asset);
	//! @brief uuid参照でAssetを設定する
	//! @param[in] id AssetのUuid
	void Set(const Uuid& id) { parameter_ = id; }

	//! @brief Assetを取得する (未設定/未解決ならnullptr)
	//! @return Assetのshared_ptr
	std::shared_ptr<T> Get() const;
	//! @brief 読み込み完了を待ってからAssetを取得する
	//! @return Assetのshared_ptr
	std::shared_ptr<T> WaitGet() const;
	//! @brief Assetを取得する. 取得できない場合は例外を投げる
	//! @return Assetのshared_ptr
	std::shared_ptr<T> Require() const;
	//! @brief 読み込み完了を待ってからRequireする
	//! @return Assetのshared_ptr
	std::shared_ptr<T> WaitRequire() const;

	//! @brief uuid参照時のみuuidをjsonへシリアライズする
	//! @return jsonデータ (それ以外はnull)
	json Serialize() const;

	//! @brief 現在の状態を表す文字列を取得する (デバッグ用)
	//! @return 状態を表す文字列
	std::string GetStr() const;

	//* operator *//

	void operator=(const std::monostate) { Reset(); }
	void operator=(const std::nullptr_t) { Reset(); }
	void operator=(const std::shared_ptr<T>& asset) { Set(asset); }
	void operator=(const Uuid& id) { Set(id); }
	void operator=(const AssetParameter<T>& other) { parameter_ = other.parameter_; }

	bool operator==(const AssetParameter<T>& other) const { return parameter_ == other.parameter_; }
	bool operator==(const Uuid& id) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Parameter parameter_ = std::monostate{};

	//=========================================================================================
	// private methods
	//=========================================================================================

	void Wait() const;
	// hack: UBaseAsset側に移動する可能性あり

};

////////////////////////////////////////////////////////////////////////////////////////////
// AssetParameter class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <Asset T>
inline AssetState AssetParameter<T>::GetState() const {
	return static_cast<AssetState>(parameter_.index());
}

template <Asset T>
inline void AssetParameter<T>::Set(const std::shared_ptr<T>& asset) {
	if (asset->HasId()) {
		parameter_ = asset->GetId(); //!< UAssetのidをセット
	}

	parameter_ = asset; //!< userが作成したUAssetをセット
}

template <Asset T>
inline std::shared_ptr<T> AssetParameter<T>::Get() const {
	switch (GetState()) {
		case AssetState::Uuid:
			return sAssetStorage->Get<T>(std::get<Uuid>(parameter_));

		case AssetState::Ptr: //!< std::shared_ptr<T>
			return std::get<std::shared_ptr<T>>(parameter_);

		default:
			return nullptr;
	}
}

template <Asset T>
inline std::shared_ptr<T> AssetParameter<T>::WaitGet() const {
	Wait();
	return Get();
}

template <Asset T>
inline std::shared_ptr<T> AssetParameter<T>::Require() const {
	switch (GetState()) {
		case AssetState::Uuid:
			return sAssetStorage->Require<T>(std::get<Uuid>(parameter_));

		case AssetState::Ptr: //!< std::shared_ptr<T>
			return std::get<std::shared_ptr<T>>(parameter_);

		default:
			StreamLogger::Exception("[AssetParameter] failed to require asset. asset is empty.");
	}
}

template <Asset T>
inline std::shared_ptr<T> AssetParameter<T>::WaitRequire() const {
	Wait();
	return Require();
}

template <Asset T>
inline json AssetParameter<T>::Serialize() const {
	switch (GetState()) {
		case AssetState::Uuid:
			return std::get<Uuid>(parameter_).Serialize();

		default:
			return nullptr;
	}
}

template <Asset T>
inline std::string AssetParameter<T>::GetStr() const {
	switch (GetState()) {
		case AssetState::Uuid:
			return std::get<Uuid>(parameter_).Serialize();

		case AssetState::Ptr:
			return "Ptr";
		
		default:
			return "null";
	}
}

template <Asset T>
inline void AssetParameter<T>::Wait() const {
	if (Empty()) {
		return;
	}

	std::shared_ptr<BaseAsset> asset = Get();

	if (asset == nullptr) {
		return;
	}

	while (!asset->IsComplete()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		RuntimeLogger::LogDebug("[AssetParameter]", std::format("waiting for asset completion. id: {}", GetStr()));
	}
}

template <Asset T>
bool AssetParameter<T>::operator==(const Uuid& id) const {
	if (std::holds_alternative<Uuid>(parameter_)) {
		return std::get<Uuid>(parameter_) == id;
	}

	return false;
}

SXAVENGER_ENGINE_NAMESPACE_END
