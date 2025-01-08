#include "AsyncAssetThreadCollection.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* external
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AsyncAssetThreadCollection class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AsyncAssetThreadCollection::Init(uint32_t threadSize) {

	threads_.resize(threadSize);

	for (uint32_t i = 0; i < threadSize; ++i) {

		threads_[i] = std::make_unique<AsyncAssetThread>();
		threads_[i]->Init([this] { //!< task getter function
			std::unique_lock<std::mutex> lock(mutex_);

			while (!tasks_.empty()) {
				auto weak = tasks_.front();
				tasks_.pop();

				if (!weak.expired()) {
					return weak.lock();
				}
			}

			return std::shared_ptr<BaseAsset>(nullptr);
		});
	}

}

void AsyncAssetThreadCollection::Term() {
	threads_.clear();
}

void AsyncAssetThreadCollection::SystemDebugGui() {
	ImGui::SeparatorText("async asset thread collection");
	ImGui::Text(std::format("remain task queue size: {}", tasks_.size()).c_str());

	ImGui::SeparatorText("threads");
	for (auto& thread : threads_) {
		thread->SystemDebugGui();
	}
}

void AsyncAssetThreadCollection::PushTask(const std::shared_ptr<BaseAsset>& task) {
	tasks_.emplace(task);
}
