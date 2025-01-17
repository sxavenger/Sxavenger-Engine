#include "AsyncAssetThread.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/Logger.h>

//* external
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AsyncAssetThread structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void AsyncAssetThread::Init() {
	DirectXThreadContext::Init(1);
}

void AsyncAssetThread::SetImGuiTableCommand() {
	ImGui::TableNextRow();

	ImGui::TableNextColumn();
	std::stringstream ss;
	ss << thread.get_id();
	ImGui::Text(ss.str().c_str());
	
	ImGui::TableNextColumn();
	bool isAvailable = (task == nullptr);

	if (isAvailable) {
		ImGui::Text(std::format("{}", isAvailable).c_str());

	} else {
		ImGui::TextDisabled(std::format("{}", isAvailable).c_str());

		if (ImGui::BeginItemTooltip()) {
			ImGui::SeparatorText("thread details");

			ImGui::Text(std::format("[runtime]: {:.1f}sec", runtime.GetElapsedTime<TimeUnit::second>().time).c_str());
			ImGui::Text(std::format("[task] filepath: {}, ptr: {}", task->GetFilepath().generic_string(), reinterpret_cast<const void*>(task.get())).c_str());

			ImGui::EndTooltip();
		}
	}
}

void AsyncAssetThread::Execute() {
	if (task == nullptr) {
		return;
	}

	runtime.Begin();

	task->Load(this);
	task = nullptr;

	runtime.End();
}

////////////////////////////////////////////////////////////////////////////////////////////
// AsyncAssetThreadCollection class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AsyncAssetThreadCollection::Init() {
	for (auto& thread : threads_) {
		thread.Init();
		thread.thread = std::thread([this, &thread]() {
			ThreadLog("[AsyncAssetThread]: Begin.");

			while (true) {
				{ //!< threadの終了またはtaskの取得
					std::unique_lock<std::mutex> lock(mutex_);
					condition_.wait(lock, [this]() { return isExit_ || !tasks_.empty(); });

					if (isExit_) { 
						break; //!< threadの終了 
					}

					while (!tasks_.empty()) {
						auto front = tasks_.front();
						tasks_.pop();

						if (!front.expired()) {
							thread.task = front.lock();
							break;
						}
					}
					
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
	ImGui::SeparatorText("async asset thread collection");
	ImGui::Text(std::format("remain task queue size: {}", tasks_.size()).c_str());

	ImGui::SeparatorText("threads");

	if (ImGui::BeginTable("## asset thread", 2, ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {
		ImGui::TableSetupColumn("thread id");
		ImGui::TableSetupColumn("available");
		ImGui::TableHeadersRow();

		for (auto& thread : threads_) {
			thread.SetImGuiTableCommand();
		}

		ImGui::EndTable();
	}
}

void AsyncAssetThreadCollection::PushTask(const std::shared_ptr<BaseAsset>& task) {
	if (task->IsComplete()) {
		return;
	}

	tasks_.push(task);
	condition_.notify_one();
}
