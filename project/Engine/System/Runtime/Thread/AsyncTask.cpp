#include "AsyncTask.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/Logger.h>

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
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		Logger::LogRuntime("waiting for task...", "tag: " + tag_, SxavengerLogger::Category::Comment);
	}
}
