#include "AsyncTask.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/RuntimeLogger.h>

//* c++
#include <thread>

////////////////////////////////////////////////////////////////////////////////////////////
// AsyncTask class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AsyncTask::Execute(const AsyncThread* thread) {
	if (function_ == nullptr) {
		return;
	}

	function_(thread);
}

void AsyncTask::Wait(Status status) const {
	while (status_ != status) {
		RuntimeLogger::LogComment("[AsyncTask]", "waiting for task... tag: " + tag_);
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}
