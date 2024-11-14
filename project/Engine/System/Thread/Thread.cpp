#include "Thread.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engin
#include <Engine/System/Logger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Interface ThreadExecution class methods
////////////////////////////////////////////////////////////////////////////////////////////

void IThreadExecution::WaitComplate() {
	while (state_ != ExecutionState::kCompleted) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// Thread class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Thread::Create(const std::function<void()>& threadFunc) {

	// threadに設定
	thread_ = std::thread(threadFunc);
}

void Thread::Term() {

	isTerm_ = true;

	if (thread_.joinable()) {
		thread_.join();
	}

	Assert(task_ == nullptr, "task is not nullptr.");
}

void Thread::ExecuteTask() {
	if (task_ == nullptr) {
		return;
	}

	if (task_->GetState() == ExecutionState::kWaiting) {
		task_->SetState(ExecutionState::kRunning);
		task_->Execute(this);
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