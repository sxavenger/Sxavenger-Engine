#include "AsyncExecutionThreadPool.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/StreamLogger.h>
#include <Engine/System/UI/SxGui.h>

//* external
#include <magic_enum.hpp>

//* c++
#include <numeric>

////////////////////////////////////////////////////////////////////////////////////////////
// TaskQueue class
////////////////////////////////////////////////////////////////////////////////////////////

void Async::ExecutionThreadPool::TaskQueue::Push(const std::shared_ptr<ExecutionTask>& task) {
	queue_[static_cast<uint8_t>(task->GetExecution())].push(task);
}

std::shared_ptr<Async::ExecutionTask> Async::ExecutionThreadPool::TaskQueue::Pop(Execution execution) {
	//!< executionに対応するキューからタスクを取得する.
	// 現在のexecutionに存在しない場合, execution下位からタスクを取得する.

	for (uint8_t i = static_cast<uint8_t>(execution); i <= static_cast<uint8_t>(Execution::Cpu); ++i) {
		if (!queue_[i].empty()) {
			std::shared_ptr<ExecutionTask> task = queue_[i].front();
			queue_[i].pop();
			return task; //!< 取得したタスクを返す.
		}
	}

	return nullptr; //!< 取得可能なタスクが存在しない.
}

bool Async::ExecutionThreadPool::TaskQueue::IsEmpty() const {
	return std::ranges::all_of(queue_, [](const std::queue<std::shared_ptr<ExecutionTask>>& q) { return q.empty(); });
}

bool Async::ExecutionThreadPool::TaskQueue::HasTask(Execution execution) const {
	for (uint8_t i = static_cast<uint8_t>(execution); i <= static_cast<uint8_t>(Execution::Cpu); ++i) {
		if (!queue_[i].empty()) {
			return true;
		}
	}

	return false; //!< executionに対応するキューおよび下位のキューにタスクが存在しない.
}

size_t Async::ExecutionThreadPool::TaskQueue::GetTaskCount(Execution execution) const {
	return queue_[static_cast<uint8_t>(execution)].size();
}

size_t Async::ExecutionThreadPool::TaskQueue::GetTotalTaskCount() const {
	return std::accumulate(queue_.begin(), queue_.end(), size_t{}, [](size_t x, const Queue& queue) { return x + queue.size(); });
}

////////////////////////////////////////////////////////////////////////////////////////////
// ExecutionThreadPool class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Async::ExecutionThreadPool::Init() {
	CreateThread(Execution::Compute, 2);
	CreateThread(Execution::Copy, 2);
	CreateThread(Execution::Cpu, 2);

	isTerminate_ = false;
}

void Async::ExecutionThreadPool::NotifyTerminate(bool isWaitForQueue) {

	if (isTerminate_) {
		return; //!< すでに終了が通知されている場合は何もしない.
	}

	if (isWaitForQueue) {
		StreamLogger::EngineThreadLog("[Async::ExecutionThreadPool] notify terminate. (wait for queue)");

		while (!queue_.IsEmpty()) { //!< キューが空になるまで待機する.
			std::this_thread::sleep_for(std::chrono::milliseconds(1)); //!< 1ms待機してから再度確認する.
		}

	} else {
		StreamLogger::EngineThreadLog("[Async::ExecutionThreadPool] notify terminate.");
	}

	std::ranges::for_each(threads_, [](ExecutionThread& thread) {
		thread.SetTerminate(); //!< threadの終了を通知する.
	});

	condition_.notify_all(); //!< 全てのスレッドに通知する.

	isTerminate_ = true;
}

void Async::ExecutionThreadPool::Shutdown() {
	NotifyTerminate(false);
	threads_.clear();
}

void Async::ExecutionThreadPool::PushTask(const std::shared_ptr<ExecutionTask>& task) {
	queue_.Push(task); //!< タスクをキューに追加する.
	StreamLogger::EngineThreadLog(
		std::format("[Async::ExecutionThreadPool] task pushed. execution: {}, tag: {}", magic_enum::enum_name(task->GetExecution()), task->GetTag())
	);

	condition_.notify_one();
}

void Async::ExecutionThreadPool::DebugGui() {

	{ //!< Queueの情報

		ImGui::Text(std::format("{} Execution - Queue", SxGui::Icon::Menu).c_str());
		ImGui::Text(std::format("Total Task: {}", queue_.GetTotalTaskCount()).c_str());

		ImGui::BeginTable("## Async Execution - Queue", 2, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders);
		ImGui::TableSetupColumn("execution");
		ImGui::TableSetupColumn("count");
		ImGui::TableHeadersRow();

		for (const auto& [value, name] : magic_enum::enum_entries<Execution>()) {
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::Text(name.data());
			ImGui::TableNextColumn();
			ImGui::Text(std::format("{}", queue_.GetTaskCount(value)).c_str());
		}

		ImGui::EndTable();
	}

	SxGui::DummyLine();

	{ //!< Threadの情報

		ImGui::Text(std::format("{} Execution - Thread", SxGui::Icon::Menu).c_str());
		ImGui::Text(std::format("Thread Count: {}", threads_.size()).c_str());

		ImGui::BeginTable("## Async Execution - Thread", 3, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders);
		ImGui::TableSetupColumn("execution");
		ImGui::TableSetupColumn("id");
		ImGui::TableSetupColumn("state");
		ImGui::TableHeadersRow();

		for (const auto& thread : threads_) {
			ImGui::TableNextRow();

			ImGui::TableNextColumn();
			ImGui::Text(magic_enum::enum_name(thread.GetExecution()).data());

			ImGui::TableNextColumn();
			std::ostringstream id;
			id << thread.GetId();
			ImGui::Text(id.str().c_str());

			ImGui::TableNextColumn();
			ExecutionThread::State state = thread.GetState();

			ImGui::BeginDisabled(state == ExecutionThread::State::Wait);
			ImGui::Text(magic_enum::enum_name(state).data());
			ImGui::EndDisabled();
		}

		ImGui::EndTable();
	}
}

std::shared_ptr<Async::ExecutionTask> Async::ExecutionThreadPool::GetTask(const ExecutionThread* thread) {

	std::unique_lock<std::mutex> lock(mutex_);
	condition_.wait(lock, [this, thread]() { return thread->IsTerminate() || !queue_.IsEmpty(); });

	if (thread->IsTerminate()) {
		return nullptr; //!< threadの終了が通知された場合, タスクを取得せずに終了する.
	}

	if (!queue_.HasTask(thread->GetExecution())) {
		if (!queue_.IsEmpty()) {
			condition_.notify_one(); //!< 他のスレッドがタスクを取得できるように通知する.
		}

		return nullptr; //!< threadのexecutionに対応するタスクが存在しない場合, タスクを取得せずに終了する.
	}

	std::shared_ptr<ExecutionTask> task = queue_.Pop(thread->GetExecution()); //!< threadのexecutionに対応するタスクを取得する.

	if (!queue_.IsEmpty()) {
		condition_.notify_one(); //!< 他のスレッドがタスクを取得できるように通知する.
	}

	if (task != nullptr) {
		StreamLogger::EngineThreadLog(
			std::format("[Async::ExecutionThread<Execution::{}>] task popped. tag: {}", magic_enum::enum_name(thread->GetExecution()), task->GetTag())
		);
	}

	return task;
}

void Async::ExecutionThreadPool::CreateThread(Execution execution, size_t count) {
	for (size_t i = 0; i < count; ++i) {
		//!< count分のスレッドを作成する.
		ExecutionThread& thread = threads_.emplace_back();
		thread.Create(execution, [this](const ExecutionThread* thread) { return GetTask(thread); });
	}
}
