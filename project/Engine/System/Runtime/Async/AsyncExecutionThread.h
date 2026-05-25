#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* async
#include "AsyncExecutionTask.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

//* c++
#include <thread>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// Async namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Async {

	////////////////////////////////////////////////////////////////////////////////////////////
	// Thread class
	////////////////////////////////////////////////////////////////////////////////////////////
	class ExecutionThread {
	public:

		////////////////////////////////////////////////////////////////////////////////////////////
		// using
		////////////////////////////////////////////////////////////////////////////////////////////

		using GetTaskFunction = std::function<std::shared_ptr<ExecutionTask>(const ExecutionThread*)>;

		////////////////////////////////////////////////////////////////////////////////////////////
		// State enum class
		////////////////////////////////////////////////////////////////////////////////////////////
		enum class State : uint8_t {
			Wait,
			Run,
			Terminate
		};

	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		ExecutionThread() = default;
		~ExecutionThread() { Shutdown(); }

		void Create(Execution execution, const GetTaskFunction& main);

		//* thread option *//

		void SetName(const std::string& name);

		void SetTerminate(bool isTerminate = true) { isTerminate_ = isTerminate; }

		void Shutdown();

		const std::thread::id GetId() const;

		State GetState() const { return state_; }

		Execution GetExecution() const { return execution_; }

		const bool IsTerminate() const { return isTerminate_; }

		//* operator [copy] (delete) *//

		ExecutionThread(const ExecutionThread&)            = delete;
		ExecutionThread& operator=(const ExecutionThread&) = delete;

		//* operator [move] *//

		ExecutionThread(ExecutionThread&&) noexcept            = default;
		ExecutionThread& operator=(ExecutionThread&&) noexcept = default;

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		//* parameter *//

		Execution execution_ = Execution::Cpu;

		//* thread *//

		std::thread thread_;
		bool isTerminate_ = false;

		State state_ = State::Wait;

		//* context *//

		std::unique_ptr<DirectXQueueContext> context_;

		//=========================================================================================
		// private variables
		//=========================================================================================

		static DirectXQueueContext::RenderQueue GetRenderQueue(Execution execution);

	};

}

SXAVENGER_ENGINE_NAMESPACE_END

