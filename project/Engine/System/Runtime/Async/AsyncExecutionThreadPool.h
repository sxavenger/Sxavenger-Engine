#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* async
#include "AsyncExecutionTask.h"
#include "AsyncExecutionThread.h"

//* engine
#include <Engine/Foundation.h>

//* c++
#include <memory>
#include <queue>
#include <array>
#include <list>
#include <mutex>
#include <condition_variable>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// Async namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Async {

	////////////////////////////////////////////////////////////////////////////////////////////
	// ExecutionThreadPool class
	////////////////////////////////////////////////////////////////////////////////////////////
	class ExecutionThreadPool {
	public:

		////////////////////////////////////////////////////////////////////////////////////////////
		// TaskQueue class
		////////////////////////////////////////////////////////////////////////////////////////////
		class TaskQueue {
		public:

			////////////////////////////////////////////////////////////////////////////////////////////
			// using
			////////////////////////////////////////////////////////////////////////////////////////////

			using Queue = std::queue<std::shared_ptr<ExecutionTask>>;
			 //!< 各Executionごとにタスクを管理するキューコンテナ.

		public:

			//=========================================================================================
			// public methods
			//=========================================================================================

			void Push(const std::shared_ptr<ExecutionTask>& task);

			std::shared_ptr<ExecutionTask> Pop(Execution execution);

			bool Empty() const;

			bool HasTask(Execution execution) const;

			Execution GetFrontExecution() const;

			size_t GetTaskCount(Execution execution) const;

			size_t GetTotalTaskCount() const;

		private:

			//=========================================================================================
			// private variables
			//=========================================================================================

			std::array<Queue, static_cast<uint8_t>(Execution::Cpu) + 1> queue_;
			//std::mutex mutex_; //!< キューへのアクセスを保護するミューテックス.

		};

		////////////////////////////////////////////////////////////////////////////////////////////
		// ThreadCondition class
		////////////////////////////////////////////////////////////////////////////////////////////
		class ThreadCondition {
		public:

			//=========================================================================================
			// public methods
			//=========================================================================================

			void Notify(Execution execution);

			void NotifyAll();

			void Wait(Execution execution, const std::function<bool()>& predicate);

		private:

			//=========================================================================================
			// private variables
			//=========================================================================================

			std::mutex mutex_;
			std::array<std::condition_variable, static_cast<uint8_t>(Execution::Cpu) + 1> conditions_;

		};

	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Init();

		void NotifyTerminate(bool isWaitForQueue = false);

		void Shutdown();

		//* task option *//

		void PushTask(const std::shared_ptr<ExecutionTask>& task);

		//* gui option *//

		void DebugGui();

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		std::list<ExecutionThread> threads_;

		TaskQueue queue_;
		ThreadCondition condition_;

		bool isTerminate_ = false;

		//=========================================================================================
		// private methods
		//=========================================================================================

		std::shared_ptr<ExecutionTask> GetTask(const ExecutionThread* thread);

		void CreateThread(Execution execution, size_t count);

	};

}

SXAVENGER_ENGINE_NAMESPACE_END

