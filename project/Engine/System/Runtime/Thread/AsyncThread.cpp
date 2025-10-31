#include "AsyncThread.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* external
#include <magic_enum.hpp>
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AsyncThread class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AsyncThread::Create(AsyncExecution execution, const MainFunction& main, const ConditionFunction& condition) {

	execution_ = execution;

	main_      = main;
	condition_ = condition;

	thread_ = std::thread([this]() {
		Logger::EngineThreadLog(std::string("[AsyncThread] begin thread. execution: ") + magic_enum::enum_name(execution_).data());

		// main loop
		while (!isTerminated_ && condition_()) {
			main_(this);
		}

		Logger::EngineThreadLog("[AsyncThread] end thread.");
	});

	if (execution_ == AsyncExecution::None) {
		return;
	}

	// contextの作成
	context_ = std::make_unique<DirectXQueueContext>();

	DirectXQueueContext::RenderQueue type = GetRenderQueueType(execution_);
	uint32_t count                        = GetAllocatorCount(execution_);

	context_->Init(count, type);

	std::wstringstream id;
	id << thread_.get_id();
	context_->SetName(std::format(L"Async Thread [id: {}]", id.str()));
}

void AsyncThread::Shutdown() {
	SetTerminate();

	if (thread_.joinable()) {
		thread_.join();
	}

	context_.reset();
}

const DirectXQueueContext* AsyncThread::GetContext() const {
	return context_.get();
}

const DirectXQueueContext* AsyncThread::RequireContext() const {
	Exception::Assert(context_ != nullptr, "thread type does not create context.");
	return context_.get();
}

DirectXQueueContext::RenderQueue AsyncThread::GetRenderQueueType(AsyncExecution execution) {
	switch (execution) {
		case AsyncExecution::Copy:
			return DirectXQueueContext::RenderQueue::Copy;

		case AsyncExecution::Compute:
			return DirectXQueueContext::RenderQueue::Compute;
	}

	Exception::Assert(false, "command list thread type error.");
	return DirectXQueueContext::RenderQueue::None; //!< error case.
}

uint32_t AsyncThread::GetAllocatorCount(AsyncExecution execution) {
	switch (execution) {
		case AsyncExecution::Copy:
			return 1;

		case AsyncExecution::Compute:
			return 2;
	}

	Exception::Assert(false, "command list thread type error.");
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////
// AsyncThreadPool class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AsyncThreadPool::Create(AsyncExecution execution, size_t size) {

	threads_.resize(size);

	// threadの作成
	for (auto& thread : threads_) {
		thread.Create(
			execution,
			[this](AsyncThread* th) {

				std::shared_ptr<AsyncTask> task = nullptr;

				{ //!< task queue にアクセス
					std::unique_lock<std::mutex> lock(mutex_);
					condition_.wait(lock, [this, th]() { return th->IsTerminated() || !queue_.empty(); });

					if (th->IsTerminated()) {
						return;
					}

					task = queue_.front();
					Logger::EngineThreadLog("[AsyncThreadPool] task poped. tag: " + queue_.front()->GetTag());
					queue_.pop();

					if (!queue_.empty()) {
						condition_.notify_one();
					}
				}

				{ //!< task の実行
					if (task->GetStatus() != AsyncTask::Status::Pending) {
						return;
					}

					th->SetStatus(AsyncThread::Status::Run);

					task->SetStatus(AsyncTask::Status::Running);
					task->Execute(th);

					if (auto context = th->GetContext()) {
						context->ExecuteAllAllocators(); //!< commandの実行
					}

					task->SetStatus(AsyncTask::Status::Completed);
					Logger::EngineThreadLog("[AsyncThread] task completed. tag: " + task->GetTag());

					th->SetStatus(AsyncThread::Status::Wait);
				}
				
			}
		);
	}
}

void AsyncThreadPool::Term() {

	// threadの終了
	for (auto& thread : threads_) {
		thread.SetTerminate();
	}

	condition_.notify_all();

	for (auto& thread : threads_) {
		thread.Shutdown();
	}

	threads_.clear();
}

void AsyncThreadPool::PushTask(const std::shared_ptr<AsyncTask>& task) {
	if (task->GetStatus() != AsyncTask::Status::None) {
		return;
	}

	std::unique_lock<std::mutex> lock(mutex_);

	task->SetStatus(AsyncTask::Status::Pending);
	queue_.emplace(task);
	Logger::EngineThreadLog("[AsyncThreadPool] task pushed. tag: " + task->GetTag());

	condition_.notify_one();
}

void AsyncThreadPool::SystemDebugGui() {
	ImGui::Text("thread count:    %u", threads_.size());
	ImGui::Text("task queue size: %u", queue_.size());

	{
		ImGui::BeginTable("## async thread pool threads table", 2, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders);
		ImGui::TableSetupColumn("id");
		ImGui::TableSetupColumn("status");
		ImGui::TableHeadersRow();

		for (const auto& thread : threads_) {
			ImGui::TableNextRow();

			{ //!< id
				ImGui::TableNextColumn();
				std::ostringstream id;
				id << thread.GetId();
				ImGui::Text(id.str().c_str());
				ImGui::SameLine();
				ImGui::Dummy({ 16.0f, 0.0f });
			}

			{ //!< status
				ImGui::TableNextColumn();
				AsyncThread::Status status = thread.GetStatus();

				ImGui::BeginDisabled(status == AsyncThread::Status::Wait);
				ImGui::Text(magic_enum::enum_name(status).data());
				ImGui::EndDisabled();
			}
		}

		ImGui::EndTable();
	}

}
