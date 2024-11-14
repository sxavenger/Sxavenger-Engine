#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* lib
#include <Lib/CXXAttributeConfig.h>

//* c++
#include <thread>
#include <mutex>
#include <functional>
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
// Interface ThreadExecution class
////////////////////////////////////////////////////////////////////////////////////////////
class IThreadExecution {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	virtual void Execute(_MAYBE_UNUSED Thread* thread) = 0;
	// TODO: commandListを入れておく

	void WaitComplate();

	ExecutionState GetState() const { return state_; }

	void SetState(ExecutionState state) { state_ = state; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	ExecutionState state_ = ExecutionState::kWaiting;

};

////////////////////////////////////////////////////////////////////////////////////////////
// Thread class
////////////////////////////////////////////////////////////////////////////////////////////
class Thread {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Thread()  = default;
	~Thread() { Term(); }

	void Create(const std::function<void()>& threadFunc);

	void Term();

	void ExecuteTask();

	//* getter *//

	bool IsTerm() const { return isTerm_; }

	//* setter *//

	void SetTask(IThreadExecution* task) { task_ = task; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::thread thread_;
	//!< commandList, fenceの追加

	bool isTerm_ = false;

	IThreadExecution* task_ = nullptr;

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

	void PushTask(IThreadExecution* task) { tasks_.push(task); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* thread parameter *//

	std::vector<std::unique_ptr<Thread>> threads_;

	std::mutex mutex_;

	//* task container *//

	std::queue<IThreadExecution*> tasks_;


};