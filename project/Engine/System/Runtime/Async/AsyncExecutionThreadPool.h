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

			//=========================================================================================
			// public methods
			//=========================================================================================

			void Push(const std::shared_ptr<ExecutionTask>& task);

			std::shared_ptr<ExecutionTask> Pop(Execution execution);

			bool IsEmpty() const;

			bool HasTask(Execution execution) const;

		private:

			//=========================================================================================
			// private variables
			//=========================================================================================

			std::array<
				std::queue<std::shared_ptr<ExecutionTask>>,
				static_cast<uint8_t>(Execution::Cpu) + 1
			> queue_;

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

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		std::list<ExecutionThread> threads_;

		std::mutex mutex_;
		std::condition_variable condition_;

		TaskQueue queue_;

		bool isTerminate_ = false;

		//=========================================================================================
		// private methods
		//=========================================================================================

		std::shared_ptr<ExecutionTask> GetTask(const ExecutionThread* thread);

		void CreateThread(Execution execution, size_t count);

	};

}

SXAVENGER_ENGINE_NAMESPACE_END

