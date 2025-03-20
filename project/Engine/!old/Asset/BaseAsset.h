#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DirectXContext.h>

//* lib
#include <Lib/CXXAttributeConfig.h>

//* c++
#include <filesystem>
#include <concepts>

////////////////////////////////////////////////////////////////////////////////////////////
// Base Asset class
////////////////////////////////////////////////////////////////////////////////////////////
class BaseAsset {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// State enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class State {
		Unloaded,
		Loading,
		Complete
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	BaseAsset()          = default;
	virtual ~BaseAsset() = default;

	virtual void Load(_MAYBE_UNUSED const DirectXThreadContext* context) = 0;

	bool IsComplete() const { return state_ == State::Complete; }

	void WaitComplete() const;

	//* getter *//

	const std::filesystem::path& GetFilepath() const { return filepath_; }

	//* setter *//

	void SetFilepath(const std::filesystem::path& filepath) { filepath_ = filepath; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* input parameter *//

	std::filesystem::path filepath_ = {};

	//=========================================================================================
	// protected methods
	//=========================================================================================

	bool CheckAndBeginLoad();

	void EndLoad() { state_ = State::Complete; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* state *//

	State state_ = State::Unloaded;

};

////////////////////////////////////////////////////////////////////////////////////////////
// concept
////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
concept BaseAssetConcept = std::derived_from<T, BaseAsset>;
