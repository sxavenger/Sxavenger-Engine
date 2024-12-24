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

////////////////////////////////////////////////////////////////////////////////////////////
// AssetState enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class AssetState : uint8_t {
	Unloaded,
	Loading,
	Loaded,
	Compiled,
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

	//* setter *//

	void SetFilepath(const std::filesystem::path& filepath) { filepath_ = filepath; }

	void Compile() { state_ = AssetState::Compiled; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* input parameter *//

	std::filesystem::path filepath_;

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
