#include "AsyncThread.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* external
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AsyncThread class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AsyncThread::Create(const std::function<void()>& threadFunction) {
	// threadの設定
	thread_ = std::thread(threadFunction);

	// directX thread contextの初期化
	DirectXThreadContext::Init(1);
}

void AsyncThread::Term() {
	isTerm_ = true;

	if (thread_.joinable()) {
		thread_.join();
	}

	DirectXThreadContext::ExecuteAllAllocators();
	Assert(task_ == nullptr, "tasks remain.");
}

void AsyncThread::SystemDebugGui() {
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
			text += std::format("[runtime]: {:.1f}sec", runtime_.GetElapsedTime<TimeUnit::s>().time) + "\n";
			text += std::format("[executing task]: {:p}", reinterpret_cast<void*>(task_.get())) + " ";

			ImGui::Text(text.c_str());

			ImGui::EndTooltip();
		}
	}
}

void AsyncThread::ExecuteTask() {
	isAvailable_ = (task_ == nullptr);

	if (task_ == nullptr) {
		return;
	}

	runtime_.Begin();
	//task_->SetState(ExecutionState::kRunning);
	//!< thread collectionで設定済み

	// taskの実行
	task_->Execute(this);
	DirectXThreadContext::ExecuteAllAllocators();

	// taskの状態を完了に設定
	task_->SetState(ExecutionState::kCompleted);
	task_ = nullptr;

	// threadの状態を利用可能に設定
	isAvailable_ = true;
	runtime_.End();
}
