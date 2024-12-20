#include "AsyncThreadCollection.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* external
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AsyncThreadCollection class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AsyncThreadCollection::Init(uint32_t threadCount) {
	for (uint32_t i = 0; i < threadCount; ++i) {
		auto& thread = threads_.emplace_back(std::make_unique<AsyncThread>());

		thread->Create([&]() {
			ThreadLog("Begin Thread.");

			while (!thread->IsTerm()) {
				{ //!< lock
					std::lock_guard<std::mutex> lock(mutex_);
					thread->SetTask(GetTask());
				}

				thread->ExecuteTask();
			}

			ThreadLog("End Thread.");
		});
	}
}

void AsyncThreadCollection::Term() {
}

void AsyncThreadCollection::PushTask(const std::weak_ptr<AsyncTask>& task) {
	std::lock_guard<std::mutex> lock(mutex_);
	tasks_.emplace(task);
}

void AsyncThreadCollection::SystemDebugGui() {
	ImGui::SeparatorText("thread collection");
	ImGui::Text(std::format("remain task queue size: {}", tasks_.size()).c_str());

	ImGui::SeparatorText("threads");
	for (auto& thread : threads_) {
		thread->SystemDebugGui();
	}
}

std::shared_ptr<AsyncTask> AsyncThreadCollection::GetTask() {
	if (tasks_.empty()) {
		return nullptr;
	}

	auto weak = tasks_.front();
	tasks_.pop();

	if (weak.expired()) {
		return nullptr;
	}

	auto task = weak.lock();

	if (task->GetState() != ExecutionState::kWaiting) {
		return nullptr;
	}

	task->SetState(ExecutionState::kRunning);
	return task;
}
