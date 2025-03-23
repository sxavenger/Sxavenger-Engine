#include "AsyncThreadLoader.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* external
#include <imgui.h>

//* c++
#include <ranges>

////////////////////////////////////////////////////////////////////////////////////////////
// AsyncThreadLoader structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void AsyncThreadLoader::Init(const std::function<void()>& function) {
	DirectXThreadContext::Init(1);
	thread_ = std::thread(function);
}

void AsyncThreadLoader::Execute() {
	if (task_ == nullptr) {
		return;
	}

	runtime_.Begin();

	task_->Load(this);
	EngineThreadLog(std::format("[AsyncThreadLoader]: task finished. filepath: {}, runtime: {}sec", task_->GetFilepath().generic_string(), runtime_.GetElapsedTime<TimeUnit::second>().time));
	task_ = nullptr;

	runtime_.End();
}

void AsyncThreadLoader::SetImGuiTableCommand() {
	ImGui::TableNextRow();

	ImGui::TableNextColumn();
	std::stringstream ss;
	ss << thread_.get_id();
	ImGui::Text(ss.str().c_str());
	
	ImGui::TableNextColumn();
	bool isAvailable = (task_ == nullptr);

	if (isAvailable) {
		ImGui::Text(std::format("{}", isAvailable).c_str());

	} else {
		ImGui::TextDisabled(std::format("{}", isAvailable).c_str());

		if (ImGui::BeginItemTooltip()) {
			ImGui::SeparatorText("thread details");

			ImGui::Text(std::format("[runtime]: {:.1f}sec", runtime_.GetElapsedTime<TimeUnit::second>().time).c_str());
			ImGui::Text(std::format("[task] filepath: {}, ptr: {}", task_->GetFilepath().generic_string(), static_cast<const void*>(task_.get())).c_str());

			ImGui::EndTooltip();
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// AsyncThreadLoaderCollection class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AsyncThreadLoaderCollection::Init() {
	for (auto& thread : threads_) {
		thread.Init([this, &thread]() {
			EngineThreadLog("[AsyncThreadLoader]: Begin Thread.");

			//* thread loop
			while (true) {
				{ //!< threadの終了またはtaskの取得
					std::unique_lock<std::mutex> lock(mutex_);
					condition_.wait(lock, [this]() { return isExit_ || !tasks_.empty(); });

					if (isExit_) {
						break; //!< threadの終了 
					}

					if (!tasks_.empty()) {
						auto front = tasks_.front();
						tasks_.pop();
						EngineThreadLog("[AsyncAssetThread]: task poped. filepath: " + front->GetFilepath().generic_string());

						thread.SetTask(front);
					}
				}

				thread.Execute();
			}

			EngineThreadLog("[AsyncThreadLoader]: End Thread.");
		});
	}
}

void AsyncThreadLoaderCollection::Term() {
	isExit_ = true;
	condition_.notify_all();

	for (auto& thread : threads_ | std::views::filter([](const AsyncThreadLoader& loader) { return loader.Joinable(); })) {
		thread.Join();
	}
}

void AsyncThreadLoaderCollection::SystemDebugGui() {
	ImGui::SeparatorText("thread collection");
	ImGui::Text(std::format("remain task queue size: {}", tasks_.size()).c_str());

	ImGui::SeparatorText("threads");

	if (ImGui::BeginTable("## thread", 2, ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {
		ImGui::TableSetupColumn("thread id");
		ImGui::TableSetupColumn("available");
		ImGui::TableHeadersRow();

		for (auto& thread : threads_) {
			thread.SetImGuiTableCommand();
		}

		ImGui::EndTable();
	}
}

void AsyncThreadLoaderCollection::PushTask(const std::shared_ptr<BaseAsset>& task) {
	std::lock_guard<std::mutex> lock(mutex_);
	tasks_.push(task);
	condition_.notify_one();
}
