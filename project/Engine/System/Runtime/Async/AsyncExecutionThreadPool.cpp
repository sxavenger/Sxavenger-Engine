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
// TaskQueue class methods
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

bool Async::ExecutionThreadPool::TaskQueue::Empty() const {
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

Async::Execution Async::ExecutionThreadPool::TaskQueue::GetFrontExecution() const {
	for (uint8_t i = 0; i <= static_cast<uint8_t>(Execution::Cpu); ++i) {
		if (!queue_[i].empty()) {
			return static_cast<Execution>(i); //!< 最も優先度の高いタスクのexecutionを返す.
		}
	}

	StreamLogger::Exception("[Async::ExecutionThreadPool::TaskQueue] no task in queue."); //!< キューにタスクが存在しない場合は例外を投げる.
}

size_t Async::ExecutionThreadPool::TaskQueue::GetTaskCount(Execution execution) const {
	return queue_[static_cast<uint8_t>(execution)].size();
}

size_t Async::ExecutionThreadPool::TaskQueue::GetTotalTaskCount() const {
	return std::accumulate(queue_.begin(), queue_.end(), size_t{}, [](size_t x, const Queue& queue) { return x + queue.size(); });
}

////////////////////////////////////////////////////////////////////////////////////////////
// ThreadCondition class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Async::ExecutionThreadPool::ThreadCondition::Notify(Execution execution) {
	for (uint8_t i = static_cast<uint8_t>(execution); i <= static_cast<uint8_t>(Execution::Cpu); ++i) {
		conditions_[i].notify_one(); //!< executionに対応する条件変数および下位の条件変数に通知する.
	}
}

void Async::ExecutionThreadPool::ThreadCondition::NotifyAll() {
	for (auto& condition : conditions_) {
		condition.notify_all(); //!< 全ての条件変数に通知する.
	}
}

void Async::ExecutionThreadPool::ThreadCondition::Wait(Execution execution, const std::function<bool()>& predicate) {
	std::unique_lock<std::mutex> lock(mutex_);
	conditions_[static_cast<uint8_t>(execution)].wait(lock, predicate); //!< executionに対応する条件変数で待機する.
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

		while (!queue_.Empty()) { //!< キューが空になるまで待機する.
			std::this_thread::sleep_for(std::chrono::milliseconds(1)); //!< 1ms待機してから再度確認する.
		}

	} else {
		StreamLogger::EngineThreadLog("[Async::ExecutionThreadPool] notify terminate.");
	}

	std::ranges::for_each(threads_, [](ExecutionThread& thread) {
		thread.SetTerminate(); //!< threadの終了を通知する.
	});

	condition_.NotifyAll(); //!< 全てのスレッドに通知する.

	isTerminate_ = true;
}

void Async::ExecutionThreadPool::Shutdown() {
	NotifyTerminate(false);
	threads_.clear();
}

void Async::ExecutionThreadPool::PushTask(const std::shared_ptr<ExecutionTask>& task) {
	// producer(タスク投入)側の処理.
	// キューに積んだだけではワーカースレッドは待機状態(condition variableでWait)のままなので,
	// 積んだ後に必ずNotifyして, そのexecutionを処理可能なスレッドを起こす必要がある.
	// Notifyは対象executionとそれ以下(下位優先度)のスレッドへ通知し, 上位スレッドが空いていれば下位タスクも拾えるようにする.
	queue_.Push(task); //!< タスクをキューに追加する.
	StreamLogger::EngineThreadLog(
		std::format("[Async::ExecutionThreadPool] task pushed. execution: {}, tag: {}", magic_enum::enum_name(task->GetExecution()), task->GetTag())
	);

	condition_.Notify(task->GetExecution());
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
	// consumer(ワーカースレッド)側の処理.
	// FIXME: thisのptrが崩壊する?s

	// 処理できるタスクが来る(またはスレッド終了が通知される)までCPUを消費せずに待機する.
	// 述語を満たさない限りWaitで眠り, PushTask/NotifyTerminateからのNotifyで起床する.
	condition_.Wait(thread->GetExecution(), [this, thread]() { return thread->IsTerminate() || queue_.HasTask(thread->GetExecution()); });

	if (thread->IsTerminate()) {
		return nullptr; //!< threadの終了が通知された場合, タスクを取得せずに終了する.
	}

	std::shared_ptr<ExecutionTask> task = queue_.Pop(thread->GetExecution()); //!< threadのexecutionに対応するタスクを取得する.

	// Notifyは1タスクにつき1スレッドしか起こさない(notify_one)ため, 複数タスクが残っている場合は取りこぼしが起きる.
	// 自分が1つ取り出した後もキューが残っていれば, 次のスレッドを連鎖的に起こして残タスクを処理させる.
	if (!queue_.Empty()) {
		condition_.Notify(queue_.GetFrontExecution()); //!< 他のスレッドがタスクを取得できるように通知する.
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
