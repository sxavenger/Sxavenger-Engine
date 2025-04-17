#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* thread
#include "AsyncTask.h"

//* engine
#include <Engine/System/DirectX/DirectXContext.h>

//* c++
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

////////////////////////////////////////////////////////////////////////////////////////////
// AsyncExecution enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class AsyncExecution : uint8_t {
	None,
	Copy,
	Compute,
};

////////////////////////////////////////////////////////////////////////////////////////////
// AsyncThread class
////////////////////////////////////////////////////////////////////////////////////////////
class AsyncThread {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////

	using ConditionFunction = std::function<bool()>;
	using MainFunction      = std::function<void(const AsyncThread*)>;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Create(
		AsyncExecution execution,
		const MainFunction& main, const ConditionFunction& condition = []() { return true; }
	);

	void SetTerminate(bool isTerminate = true) { isTerminated_ = isTerminate; }

	void Shutdown();

	//* thread option *//

	const DirectXThreadContext* GetContext() const;

	const bool IsTerminated() const { return isTerminated_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::thread thread_;

	bool isTerminated_ = false;

	ConditionFunction condition_;
	MainFunction      main_;

	AsyncExecution execution_ = AsyncExecution::None;

	//* context *//

	std::unique_ptr<DirectXThreadContext> context_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	static D3D12_COMMAND_LIST_TYPE GetCommandListType(AsyncExecution execution);
	static uint32_t GetAllocatorCount(AsyncExecution execution);

};

////////////////////////////////////////////////////////////////////////////////////////////
// AsyncThreadPool class
////////////////////////////////////////////////////////////////////////////////////////////
class AsyncThreadPool {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Create(AsyncExecution execution, size_t size);

	void Term();

	//* task option *//

	void SetTask(const std::shared_ptr<AsyncTask>& task);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* threads *//

	std::vector<AsyncThread> threads_;

	std::mutex mutex_;
	std::condition_variable condition_;

	//* task queue *//

	std::queue<std::shared_ptr<AsyncTask>> queue_;
	

};
