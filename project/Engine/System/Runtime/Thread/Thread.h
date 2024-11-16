#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DirectXContext.h>

//* lib
#include <Lib/CXXAttributeConfig.h>

//* c++
#include <thread>
#include <functional>
#include <mutex>
#include <queue>

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class Thread;

////////////////////////////////////////////////////////////////////////////////////////////
// ExecutionState enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class ExecutionState {
	kWaiting,   //!< 待機中
	kRunning,   //!< 実行中
	kCompleted, //!< 完了
};

////////////////////////////////////////////////////////////////////////////////////////////
// BaseTaskExecution class
////////////////////////////////////////////////////////////////////////////////////////////
class BaseTaskExecution {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	virtual void Execute(_MAYBE_UNUSED const Thread* const thread) = 0;

	void WaitCompleted();

	bool IsCompleted() { return state_ == ExecutionState::kCompleted; }

	//* getter and setter *//

	ExecutionState GetState() const { return state_; }

	void SetState(ExecutionState state) { state_ = state; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	ExecutionState state_ = ExecutionState::kWaiting;
};

////////////////////////////////////////////////////////////////////////////////////////////
// Thread class
////////////////////////////////////////////////////////////////////////////////////////////
class Thread
	: public DirectXThreadContext {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Thread()  = default;
	~Thread() { Term(); }

	virtual void Create(const std::function<void()>& threadFunc);

	void Term();

	void ExecuteTask();

	//* getter *//

	bool IsTerm() const { return isTerm_; }

	//* setter *//

	void SetTask(BaseTaskExecution* task) { task_ = task; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* thread paramter *//

	std::thread thread_;

	bool isTerm_ = false;

	//* task parameter *//

	BaseTaskExecution* task_ = nullptr;

};

////////////////////////////////////////////////////////////////////////////////////////////
// ThreadCollection class
////////////////////////////////////////////////////////////////////////////////////////////
class ThreadCollection {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ThreadCollection()  = default;
	~ThreadCollection() { Term(); }

	void Init(uint32_t threadCount);

	void Term();

	void PushTask(BaseTaskExecution* task) { tasks_.emplace(task); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* thread parameter *//

	std::vector<std::unique_ptr<Thread>> threads_;

	std::mutex mutex_;

	//* task container *//

	std::queue<BaseTaskExecution*> tasks_;


};