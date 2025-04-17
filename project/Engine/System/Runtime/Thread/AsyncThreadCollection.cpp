#include "AsyncThreadCollection.h"

////////////////////////////////////////////////////////////////////////////////////////////
// AsyncThreadCollection class
////////////////////////////////////////////////////////////////////////////////////////////

void AsyncThreadCollection::Init() {
	// thread pool の生成
	pools_[static_cast<uint8_t>(AsyncExecution::None)].Create(AsyncExecution::None, 2);
	pools_[static_cast<uint8_t>(AsyncExecution::Copy)].Create(AsyncExecution::Copy, 2);
	pools_[static_cast<uint8_t>(AsyncExecution::Compute)].Create(AsyncExecution::Compute, 2);
}

void AsyncThreadCollection::Term() {
	for (auto& pool : pools_) {
		pool.Term();
	}
}

void AsyncThreadCollection::PushTask(AsyncExecution execution, const std::shared_ptr<AsyncTask>& task) {
	pools_[static_cast<uint8_t>(execution)].PushTask(task);
}
