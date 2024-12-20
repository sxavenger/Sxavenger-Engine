#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/CXXAttributeConfig.h>

//* c++
#include <functional>

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class AsyncThread;

////////////////////////////////////////////////////////////////////////////////////////////
// using
////////////////////////////////////////////////////////////////////////////////////////////
using AsyncThreadExecuteFunction = std::function<void(_MAYBE_UNUSED const AsyncThread* const)>;

////////////////////////////////////////////////////////////////////////////////////////////
// ExecutionState enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class ExecutionState {
	kNone,      //!< 設定なし
	kWaiting,   //!< 待機中
	kRunning,   //!< 実行中
	kCompleted, //!< 完了
};

////////////////////////////////////////////////////////////////////////////////////////////
// AsyncTask class
////////////////////////////////////////////////////////////////////////////////////////////
class AsyncTask {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	AsyncTask()  = default;
	~AsyncTask() = default;

	//* async thread option *//

	void Execute(const AsyncThread* const thread);

	//* task option *//

	void SetTask(const AsyncThreadExecuteFunction& function) { function_ = function; }

	//* state option *//

	void WaitCompleted() const;

	bool IsCompleted() const { return state_ == ExecutionState::kCompleted; }

	ExecutionState GetState() const { return state_; }

	void SetState(ExecutionState state) { state_ = state; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	AsyncThreadExecuteFunction function_;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	ExecutionState state_ = ExecutionState::kNone;

};
