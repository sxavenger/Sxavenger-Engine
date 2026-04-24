#include "AsyncExecutionTask.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/RuntimeLogger.h>

//* c++
#include <thread>

////////////////////////////////////////////////////////////////////////////////////////////
// ExecutionTask class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Async::ExecutionTask::Execute(const DirectXQueueContext* context) const {
	if (function_ == nullptr) {
		return;
	}

	function_(this, context);
}

void Async::ExecutionTask::SetFunction(Execution execution, const ExecutionFunction& function) {
	execution_ = execution;
	function_  = function;
}

void Async::ExecutionTask::Wait(Status status) const {
	while (status_ != status) {
		RuntimeLogger::LogDebug("[Async::ExecutionTask]", "waiting for task... tag: " + tag_);
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}
