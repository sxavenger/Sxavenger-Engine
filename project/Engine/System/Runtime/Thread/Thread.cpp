#include "Thread.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// TaskThreadExecution class methods
////////////////////////////////////////////////////////////////////////////////////////////

void TaskThreadExecution::Execute(const Thread* const thread) {
	task_(thread);
}

void TaskThreadExecution::WaitCompleted() const {
}

////////////////////////////////////////////////////////////////////////////////////////////
// Thread class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Thread::Create(const std::function<void()>& threadFunc) {
	// threadに設定
	thread_ = std::thread(threadFunc);

	DirectXThreadContext::Init(1);
}

void Thread::Term() {
	isTerm_ = true;

	if (thread_.joinable()) {
		thread_.join();
	}

	DirectXThreadContext::ExecuteAllAllocators();
	Assert(task_ == nullptr, "tasks remain.");
}

void Thread::ExecuteTask() {
	if (task_ == nullptr) {
		return;
	}

	if (task_->GetState() == ExecutionState::kWaiting) {
		task_->SetState(ExecutionState::kRunning);

		task_->Execute(this);
		DirectXThreadContext::ExecuteAllAllocators();

		task_->SetState(ExecutionState::kCompleted);
	}

	task_ = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////
// ThreadCollection class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ThreadCollection::Init(uint32_t threadCount) {

	// threadの設定
	threads_.resize(threadCount);

	for (auto& thread : threads_) {
		thread = std::make_unique<Thread>();

		thread->Create([&]() {
			ThreadLog("Begin Thread.");

			while (true) {
				{ //!< lock
					std::lock_guard<std::mutex> lock(mutex_);

					if (!tasks_.empty()) {
						thread->SetTask(tasks_.front());
						tasks_.pop();
					}
				}

				thread->ExecuteTask();

				if (thread->IsTerm()) {
					break;
				}
			}

			ThreadLog("End Thread.");
		});
	}
}

void ThreadCollection::Term() {
}
