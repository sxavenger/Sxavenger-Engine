#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>
#include <Engine/System/Runtime/Thread/AsyncTask.h>
#include <Engine/System/Runtime/Thread/AsyncThread.h>
#include <Engine/Editor/Editors/InspectorEditor.h>

//* lib
#include <Lib/Adapter/Uuid/Uuid.h>

//* c++
#include <filesystem>
#include <any>

////////////////////////////////////////////////////////////////////////////////////////////
// UBaseContent class
////////////////////////////////////////////////////////////////////////////////////////////
class UBaseContent
	: public BaseInspector, public AsyncTask {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	UBaseContent() = default;
	virtual ~UBaseContent() = default;

	//* async task option *//

	virtual void AsyncLoad(_MAYBE_UNUSED const DirectXQueueContext* context) = 0;

	virtual AsyncExecution GetAsyncExecution() const = 0;

	void Execute(const AsyncThread* thread) override;

	//* uuid option *//

	virtual void AttachUuid() = 0;

	//* state option *//

	bool IsComplete() const { return AsyncTask::IsCompleted(); }

	void WaitComplete() const;

	//* parameter option *//

	void SetFilepath(const std::filesystem::path& filepath);

	const std::filesystem::path& GetFilepath() const;

	void SetParam(const std::any& param) { param_ = param; }

	const std::any& GetParam() const { return param_; }

	//* inspector option *//

	virtual void ShowInspector() override;

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	std::filesystem::path filepath_; //!< content filepath.
	std::any param_;                 //!< content parameter.

	//=========================================================================================
	// protected methods
	//=========================================================================================

	//* helper method *//

	void CheckExist() const;

	std::filesystem::path GetContentPath() const;

};

////////////////////////////////////////////////////////////////////////////////////////////
// concept
////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
concept UContentConcept = std::derived_from<T, UBaseContent> && !std::is_same_v<T, UBaseContent>;
