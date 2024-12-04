#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DirectXContext.h>
#include <Engine/System/UI/ISystemDebugGui.h>
#include <Engine/System/Runtime/Performance/DeltaTimePoint.h>
#include <Engine/System/Runtime/Performance/Performance.h>

//* lib
#include <Lib/CXXAttributeConfig.h>

//* c++
#include <thread>
#include <functional>
#include <mutex>
#include <queue>
#include <memory>

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class Thread;

////////////////////////////////////////////////////////////////////////////////////////////
// using
////////////////////////////////////////////////////////////////////////////////////////////
using ThreadExecuteFunction = std::function<void(_MAYBE_UNUSED const Thread* const)>;

////////////////////////////////////////////////////////////////////////////////////////////
// ExecutionState enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class ExecutionState {
	kWaiting,   //!< 待機中
	kRunning,   //!< 実行中
	kCompleted, //!< 完了
};

////////////////////////////////////////////////////////////////////////////////////////////
// TaskThreadExecution class
////////////////////////////////////////////////////////////////////////////////////////////
class TaskThreadExecution {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	TaskThreadExecution()  = default;
	~TaskThreadExecution() = default;

	//* task option *//

	void Execute(const Thread* const thread);

	void SetTask(const ThreadExecuteFunction& task) { task_ = task; }

	//* state option *//

	void WaitCompleted() const;

	bool IsCompleted() const { return state_ == ExecutionState::kCompleted; }

	ExecutionState GetState() const { return state_; }

	void SetState(ExecutionState state) { state_ = state; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	ThreadExecuteFunction task_;

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
	: public DirectXThreadContext, public ISystemDebugGui {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Thread()  = default;
	~Thread() { Term(); }

	virtual void Create(const std::function<void()>& threadFunc);

	void Term();

	void ExecuteTask();

	void SystemDebugGui() override;

	//* getter *//

	bool IsTerm() const { return isTerm_; }

	//* setter *//

	void SetTask(const std::shared_ptr<TaskThreadExecution>& task) { task_ = task; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* thread paramter *//

	std::thread thread_;

	bool isTerm_      = false;
	bool isAvailable_ = true;

	//* task parameter *//

	std::shared_ptr<TaskThreadExecution> task_ = nullptr;
	

	RunTimeTracker runtime_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// ThreadCollection class
////////////////////////////////////////////////////////////////////////////////////////////
class ThreadCollection
	: public ISystemDebugGui {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ThreadCollection()  = default;
	~ThreadCollection() { Term(); }

	void Init(uint32_t threadCount);

	void Term();

	void PushTask(const std::shared_ptr<TaskThreadExecution>& task) { tasks_.emplace(task); }

	void SystemDebugGui() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* thread parameter *//

	std::vector<std::unique_ptr<Thread>> threads_;

	std::mutex mutex_;

	//* task container *//

	std::queue<std::shared_ptr<TaskThreadExecution>> tasks_;
	// FIXME: 所有者が破棄されると未定義動作になる.


};