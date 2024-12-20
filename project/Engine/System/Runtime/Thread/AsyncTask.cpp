#include "AsyncTask.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <thread>
#include <chrono>

////////////////////////////////////////////////////////////////////////////////////////////
// AsyncTask class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AsyncTask::Execute(const AsyncThread* const thread) {
	function_(thread);
	function_ = nullptr;
}

void AsyncTask::WaitCompleted() const {
	while (state_ != ExecutionState::kCompleted) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}
