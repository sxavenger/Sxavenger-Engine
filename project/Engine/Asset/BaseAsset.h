#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DirectXContext.h>

//* lib
#include <Lib/CXXAttributeConfig.h>

//* c++
#include <cstdint>
#include <filesystem>
#include <mutex>
#include <concepts>

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class AssetCollection;

////////////////////////////////////////////////////////////////////////////////////////////
// AssetState enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class AssetState : uint8_t {
	Unloaded,
	Loading,
	Loaded,
	Complete //!< 同threadでのLoadの場合, この状態になることはない
};

////////////////////////////////////////////////////////////////////////////////////////////
// Base Asset class
////////////////////////////////////////////////////////////////////////////////////////////
class BaseAsset {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	BaseAsset()          = default;
	virtual ~BaseAsset() = default;

	virtual void Load(_MAYBE_UNUSED const DirectXThreadContext* context) = 0;

	//* state option *//

	void WaitComplete() const; //!< 同threadでのLoadの場合, この関数は使わない

	AssetState GetAssetState() const { return state_; }

	//* setter *//

	void SetCollection(AssetCollection* collection) { collection_ = collection; }

	void SetFilepath(const std::filesystem::path& filepath) { filepath_ = filepath; }

	void Complete() { state_ = AssetState::Complete; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* input parameter *//

	std::filesystem::path filepath_;

	//* asset collection *//

	AssetCollection* collection_ = nullptr;

	//=========================================================================================
	// protected methods
	//=========================================================================================

	bool CheckAndBeginLoad();

	void EndLoad() { state_ = AssetState::Loaded; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* mutex *//

	std::mutex mutex_;

	//* state *//

	AssetState state_ = AssetState::Unloaded;

};

////////////////////////////////////////////////////////////////////////////////////////////
// concept
////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
concept BaseAssetConcept = std::is_base_of_v<BaseAsset, T>;

template <class T>
concept DerivedFromBaseAssetConcept = std::derived_from<T, BaseAsset>;
