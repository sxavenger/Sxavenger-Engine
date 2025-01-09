#include "AsyncAssetThread.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/Logger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AsyncAssetThread structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void AsyncAssetThread::SystemDebugGui() {
}

void AsyncAssetThread::Execute() {
	if (task == nullptr) {
		return;
	}

	task->Load(this);
	task = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////
// AsyncAssetThreadCollection class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AsyncAssetThreadCollection::Init() {
	for (auto& thread : threads_) {
		thread.thread = std::thread([this, &thread]() {
			ThreadLog("[AsyncAssetThread]: Begin.");

			while (true) {
				{ //!< threadの終了またはtaskの取得
					std::unique_lock<std::mutex> lock(mutex_);
					condition_.wait(lock, [this]() { return isExit_ || !tasks_.empty(); });

					
					if (isExit_) { 
						break; //!< threadの終了 
					}

					thread.task = tasks_.front();
					tasks_.pop();
				}

				thread.Execute();
			}

			ThreadLog("[AsyncAssetThread]: End.");
		});
	}
}

void AsyncAssetThreadCollection::Term() {
	isExit_ = true;
	condition_.notify_all();

	for (auto& thread : threads_) {
		thread.thread.join();
	}
}

void AsyncAssetThreadCollection::SystemDebugGui() {
}

void AsyncAssetThreadCollection::PushTask(const std::shared_ptr<BaseAsset>& task) {
	if (task->IsComplete()) {
		return;
	}

	tasks_.push(task);
	condition_.notify_one();
}
