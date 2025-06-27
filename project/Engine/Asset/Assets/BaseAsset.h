#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DirectXContext.h>
#include <Engine/System/Runtime/Thread/AsyncTask.h>
#include <Engine/System/Runtime/Thread/AsyncThread.h>
#include <Engine/Editor/Editors/InspectorEditor.h>

//* lib
#include <Lib/CXXAttributeConfig.h>

//* c++
#include <filesystem>
#include <concepts>
#include <optional>
#include <any>

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

	//* async task option *//

	virtual void Load(_MAYBE_UNUSED const DirectXThreadContext* context) = 0;

	virtual AsyncExecution GetAsyncExecution() const = 0;

	void Execute(const AsyncThread* thread) override;

	//* state option *//

	bool IsComplete() const { return AsyncTask::IsCompleted(); }

	void WaitComplete() const;

	//* parameter option *//

	void SetFilepath(const std::filesystem::path& filepath);

	const std::filesystem::path& GetFilepath() const { return filepath_; }

	void SetParam(const std::any& param) { param_ = param; }

	const std::any& GetParam() const { return param_; }

	//* inspector option *//

	virtual void ShowInspector() override;

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* parameter *//

	std::filesystem::path filepath_;
	std::any param_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// concept
////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
concept BaseAssetConcept = std::derived_from<T, BaseAsset>;
