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
enum class AssetState : uint8_t {
	Monostate, //!< monostate:          空の状態
	Uuid,      //!< Uuid:               uassetのid
	Ptr,       //!< std::shared_ptr<T>: userが作成したuasset
};

////////////////////////////////////////////////////////////////////////////////////////////
// AssetParameter class
////////////////////////////////////////////////////////////////////////////////////////////
template <AssetConcept T>
class AssetParameter {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////

	using UAssetType = T;

	////////////////////////////////////////////////////////////////////////////////////////////
	// Parameter variant
	////////////////////////////////////////////////////////////////////////////////////////////
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

	AssetState GetState() const;

	bool Empty() const { return GetState() == AssetState::Monostate; }

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
// UAssetParameter class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template <AssetConcept T>
inline AssetState AssetParameter<T>::GetState() const {
	return static_cast<AssetState>(parameter_.index());
}

template <AssetConcept T>
inline void AssetParameter<T>::Set(const std::shared_ptr<T>& asset) {
	if (asset->HasId()) {
		parameter_ = asset->GetId(); //!< UAssetのidをセット
	}

	parameter_ = asset; //!< userが作成したUAssetをセット
}

template <AssetConcept T>
inline std::shared_ptr<T> AssetParameter<T>::Get() const {
	switch (GetState()) {
		case AssetState::Uuid:
			return sAssetStorage->GetAsset<T>(std::get<Uuid>(parameter_));

		case AssetState::Ptr: //!< std::shared_ptr<T>
			return std::get<std::shared_ptr<T>>(parameter_);

		default:
			return nullptr;
	}
}

template <AssetConcept T>
inline std::shared_ptr<T> AssetParameter<T>::WaitGet() const {
	Wait();
	return Get();
}

template <AssetConcept T>
inline std::shared_ptr<T> AssetParameter<T>::Require() const {
	StreamLogger::AssertA(!Empty(), "AssetParameter is empty.");

	return Get();
}

template <AssetConcept T>
inline std::shared_ptr<T> AssetParameter<T>::WaitRequire() const {
	Wait();
	return Require();
}

template <AssetConcept T>
inline json AssetParameter<T>::Serialize() const {
	switch (GetState()) {
		case AssetState::Uuid:
			return std::get<Uuid>(parameter_).Serialize();

		default:
			return nullptr;
	}
}

template <AssetConcept T>
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

template <AssetConcept T>
inline void AssetParameter<T>::Wait() const {
	if (Empty()) {
		return;
	}

	std::shared_ptr<BaseAsset> asset = Get();

	while (!asset->IsComplete()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		RuntimeLogger::LogComment("[AssetParameter]", std::format("waiting for asset completion. id: {}", GetStr()));
	}
}

template <AssetConcept T>
bool AssetParameter<T>::operator==(const Uuid& id) const {
	if (std::holds_alternative<Uuid>(parameter_)) {
		return std::get<Uuid>(parameter_) == id;
	}

	return false;
}

SXAVENGER_ENGINE_NAMESPACE_END
