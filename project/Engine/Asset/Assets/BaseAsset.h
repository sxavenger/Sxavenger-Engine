#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DirectXContext.h>
#include <Engine/System/Runtime/Thread/AsyncTask.h>
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
	: public BaseInspector, public AsyncTask {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	BaseAsset()          = default;
	virtual ~BaseAsset() = default;

	virtual void Load(_MAYBE_UNUSED const DirectXThreadContext* context) = 0;

	void Execute(const AsyncThread* thread) override;

	//* state option *//

	bool IsComplete() const { return AsyncTask::GetStatus() == AsyncTask::Status::Completed; }

	void WaitComplete() const;

	//* filepath option *//

	void SetFilepath(const std::filesystem::path& filepath);

	const std::filesystem::path& GetFilepath() const { return filepath_; }

	//* inspector option *//

	virtual void ShowInspector() override;

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	std::filesystem::path filepath_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// concept
////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
concept BaseAssetConcept = std::derived_from<T, BaseAsset>;
