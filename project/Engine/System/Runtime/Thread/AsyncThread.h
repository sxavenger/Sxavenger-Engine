#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* thread
#include "AsyncTask.h"

//* engine
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>
#include <Engine/System/UI/ISystemDebugGui.h>

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
static inline constexpr uint8_t kAsyncExecutionCount = static_cast<uint8_t>(AsyncExecution::Compute) + 1;

////////////////////////////////////////////////////////////////////////////////////////////
// AsyncThread class
////////////////////////////////////////////////////////////////////////////////////////////
class AsyncThread {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////

	using ConditionFunction = std::function<bool()>;
	using MainFunction      = std::function<void(AsyncThread*)>;

	////////////////////////////////////////////////////////////////////////////////////////////
	// Status enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Status : bool {
		Wait,
		Run,
	};

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

	void SetStatus(Status status) { status_ = status; }

	Status GetStatus() const { return status_; }

	const DirectXQueueContext* GetContext() const;

	const DirectXQueueContext* RequireContext() const;

	const bool IsTerminated() const { return isTerminated_; }

	const std::thread::id GetId() const { return thread_.get_id(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::thread thread_;

	bool isTerminated_ = false;

	ConditionFunction condition_;
	MainFunction      main_;

	AsyncExecution execution_ = AsyncExecution::None;

	Status status_ = Status::Wait;

	//* context *//

	std::unique_ptr<DirectXQueueContext> context_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	static DirectXQueueContext::RenderQueue GetRenderQueueType(AsyncExecution execution);

	static uint32_t GetAllocatorCount(AsyncExecution execution);

};

////////////////////////////////////////////////////////////////////////////////////////////
// AsyncThreadPool class
////////////////////////////////////////////////////////////////////////////////////////////
class AsyncThreadPool
	: public ISystemDebugGui {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Create(AsyncExecution execution, size_t size);

	void Term();

	//* task option *//

	void PushTask(const std::shared_ptr<AsyncTask>& task);

	//* debug gui *//

	void SystemDebugGui() override;

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
