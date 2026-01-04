#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* thread
#include "AsyncTask.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>
#include <Engine/System/UI/ISystemDebugGui.h>

//* c++
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

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
//! @brief 非同期スレッドクラス.
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

	//! @brief threadを終了する
	void Shutdown();

	//* thread option *//

	void SetStatus(Status status) { status_ = status; }

	Status GetStatus() const { return status_; }

	const DirectXQueueContext* GetContext() const;

	//! @brief threadのcontextを要求する
	//! @throw contextが存在しない場合(AsyncExecution::None)に例外をスローする
	const DirectXQueueContext* RequireContext() const;

	void SetTerminate(bool isTerminate = true) { isTerminated_ = isTerminate; }

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

	void SetTerminate();

	void Shutdown();

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

SXAVENGER_ENGINE_NAMESPACE_END
