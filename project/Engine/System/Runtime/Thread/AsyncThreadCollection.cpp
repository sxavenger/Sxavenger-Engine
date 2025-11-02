#include "AsyncThreadCollection.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* externals
#include <magic_enum.hpp>
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AsyncThreadCollection class
////////////////////////////////////////////////////////////////////////////////////////////

void AsyncThreadCollection::Init() {
	// thread pool の生成
	pools_[static_cast<uint8_t>(AsyncExecution::None)].Create(AsyncExecution::None, 2);
	pools_[static_cast<uint8_t>(AsyncExecution::Copy)].Create(AsyncExecution::Copy, 3);
	pools_[static_cast<uint8_t>(AsyncExecution::Compute)].Create(AsyncExecution::Compute, 1);
}

void AsyncThreadCollection::SetTerminate() {
	for (auto& pool : pools_) {
		pool.SetTerminate();
	}
}

void AsyncThreadCollection::Shutdown() {
	SetTerminate();

	for (auto& pool : pools_) {
		pool.Shutdown();
	}
}

void AsyncThreadCollection::PushTask(AsyncExecution execution, const std::shared_ptr<AsyncTask>& task) {
	pools_[static_cast<uint8_t>(execution)].PushTask(task);
}

void AsyncThreadCollection::SystemDebugGui() {
	// thread pool ごとの情報表示
	for (uint8_t i = 0; i < kAsyncExecutionCount; ++i) {
		ImGui::SeparatorText(std::format("Async Thread Pool [{}]", magic_enum::enum_name(static_cast<AsyncExecution>(i))).c_str());
		pools_[i].SystemDebugGui();
	}
}
