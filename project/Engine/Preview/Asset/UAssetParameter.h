#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* uasset
#include "UBaseAsset.h"
#include "UAssetStorage.h"

//* engine
#include <Engine/System/Utility/Logger.h>

//* lib
#include <Lib/Adapter/Uuid/Uuid.h>
#include <Lib/Adapter/Json/JsonHandler.h>

//* c++
#include <variant>
#include <memory>
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////
// UAssetParameter class
////////////////////////////////////////////////////////////////////////////////////////////
template <UAssetConcept T>
class UAssetParameter {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////

	using UAssetType = T;

	using Parameter = std::variant<std::monostate, Uuid, std::shared_ptr<T>>;
	//! monostate: 空の状態
	//! Uuid: uassetのid
	//! std::shared_ptr<T>: userが作成したuasset

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	UAssetParameter() = default;
	UAssetParameter(const std::nullptr_t) { Reset(); }
	UAssetParameter(const std::shared_ptr<T>& asset) { Set(asset); }
	UAssetParameter(const Uuid& id) { Set(id); }

	bool Empty() const { return std::holds_alternative<std::monostate>(parameter_); }

	void Reset() { parameter_ = std::monostate{}; }

	void Set(const std::shared_ptr<T>& asset);
	void Set(const Uuid& id) { parameter_ = id; }

	std::shared_ptr<T> Get() const;
	std::shared_ptr<T> WaitGet() const;
	std::shared_ptr<T> Require() const;
	std::shared_ptr<T> WaitRequire() const;

	json Serialize() const;

	std::string GetStr() const;

	//* operator *//

	void operator=(const std::monostate) { Reset(); }
	void operator=(const std::nullptr_t) { Reset(); }
	void operator=(const std::shared_ptr<T>& asset) { Set(asset); }
	void operator=(const Uuid& id) { Set(id); }
	void operator=(const UAssetParameter<T>& other) { parameter_ = other.parameter_; }

	bool operator==(const UAssetParameter<T>& other) const { return parameter_ == other.parameter_; }
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
// UAssetParameter class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <UAssetConcept T>
inline void UAssetParameter<T>::Set(const std::shared_ptr<T>& asset) {
	if (asset->HasId()) {
		parameter_ = asset->GetId(); //!< UAssetのidをセット
	}

	parameter_ = asset; //!< userが作成したUAssetをセット
}

template <UAssetConcept T>
inline std::shared_ptr<T> UAssetParameter<T>::Get() const {
	switch (parameter_.index()) {
		default:
			return nullptr;

		case 1: //!< Uuid
			return sUAssetStorage->GetAsset<T>(std::get<Uuid>(parameter_));

		case 2: //!< std::shared_ptr<T>
			return std::get<std::shared_ptr<T>>(parameter_);
	}
}

template <UAssetConcept T>
inline std::shared_ptr<T> UAssetParameter<T>::WaitGet() const {
	Wait();
	return Get();
}

template <UAssetConcept T>
inline std::shared_ptr<T> UAssetParameter<T>::Require() const {
	Exception::Assert(!Empty(), "UAssetParameter is empty.");

	return Get();
}

template <UAssetConcept T>
inline std::shared_ptr<T> UAssetParameter<T>::WaitRequire() const {
	Wait();
	return Require();
}

template <UAssetConcept T>
inline json UAssetParameter<T>::Serialize() const {
	switch (parameter_.index()) {
		case 1: //!< Uuid
			return std::get<Uuid>(parameter_).Serialize();

		default:
			return nullptr;
	}
}

template <UAssetConcept T>
inline std::string UAssetParameter<T>::GetStr() const {
	switch (parameter_.index()) {
		case 1: //!< Uuid
			return std::get<Uuid>(parameter_).Serialize();
		
		default:
			return "null";
	}
}

template <UAssetConcept T>
inline void UAssetParameter<T>::Wait() const {
	if (Empty()) {
		return;
	}

	std::shared_ptr<UBaseAsset> asset = Get();

	while (!asset->IsComplete()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		Logger::CommentRuntime(std::format("[UAssetParameter]: waiting for asset completion. id: {}", GetStr()));
	}
}

template <UAssetConcept T>
bool UAssetParameter<T>::operator==(const Uuid& id) const {
	if (std::holds_alternative<Uuid>(parameter_)) {
		return std::get<Uuid>(parameter_) == id;
	}

	return false;
}
