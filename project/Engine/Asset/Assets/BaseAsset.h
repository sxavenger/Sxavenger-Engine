#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DirectXContext.h>
#include <Engine/Editor/Editors/InspectorEditor.h>

//* lib
#include <Lib/CXXAttributeConfig.h>

//* c++
#include <filesystem>
#include <concepts>

////////////////////////////////////////////////////////////////////////////////////////////
// BaseAsset class
////////////////////////////////////////////////////////////////////////////////////////////
class BaseAsset
	: public BaseInspector {
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

	//* state option *//

	bool IsComplete() const { return state_ == State::Complete; }

	void WaitComplete() const;

	//* filepath option *//

	void SetFilepath(const std::filesystem::path& filepath) { filepath_ = filepath; }

	const std::filesystem::path& GetFilepath() const { return filepath_; }

	//* inspector option *//

	virtual void ShowInspector() override;

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	std::filesystem::path filepath_;

	//=========================================================================================
	// protected methods
	//=========================================================================================

	bool IsLoaded() const;

	void SetState(State state) { state_ = state; }

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
