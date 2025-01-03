#include "AsyncAssetThread.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* external
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AsyncAssetThread class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AsyncAssetThread::Init(const std::function<std::shared_ptr<BaseAsset>()>& getter) {

	DirectXThreadContext::Init(1);

	thread_ = std::thread([this, getter] {
		ThreadLog("[AsyncAssetThread]: Begin thread.");

		while (!isTerm_) {
			task_ = getter();
			Execute();
		}

		ThreadLog("[AsyncAssetThread]: End thread.");
	});
}

void AsyncAssetThread::Term() {

	isTerm_ = true;

	if (thread_.joinable()) {
		thread_.join();
	}
}

void AsyncAssetThread::SystemDebugGui() {
	std::stringstream ss;
	ss << "[thread id]: " << thread_.get_id() << " ";
	ss << std::format("[available]: {}", isAvailable_) << " ";

	if (isAvailable_) {
		ImGui::Text(ss.str().c_str());

	} else {
		ImGui::TextDisabled(ss.str().c_str());

		if (ImGui::BeginItemTooltip()) {
			ImGui::SeparatorText("thread details");

			std::string text = "";
			text += std::format("[runtime]: {:.1f}sec", runtime_.GetElapsedTime<TimeUnit::second>().time) + "\n";
			text += std::format("[task]: {:p}", reinterpret_cast<void*>(task_.get())) + " ";

			ImGui::Text(text.c_str());

			ImGui::EndTooltip();
		}
	}
}

void AsyncAssetThread::Execute() {
	isAvailable_ = (task_ == nullptr);

	if (task_ == nullptr) {
		return;
	}

	runtime_.Begin();

	task_->Load(this);

	task_        = nullptr;
	isAvailable_ = false;

	runtime_.End();
}
