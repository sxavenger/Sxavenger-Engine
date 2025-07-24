#include "AsyncThread.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* external
#include <magic_enum.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// AsyncThread class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AsyncThread::Create(AsyncExecution execution, const MainFunction& main, const ConditionFunction& condition) {

	execution_ = execution;

	main_      = main;
	condition_ = condition;

	thread_ = std::thread([this]() {
		Logger::EngineThreadLog(std::string("[AsyncThread] begin thread. execution: ") + magic_enum::enum_name(execution_).data());
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
			[this](const AsyncThread* th) {

				std::shared_ptr<AsyncTask> task = nullptr;

				{
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

				if (task->GetStatus() != AsyncTask::Status::Pending) {
					return;
				}

				// todo: thread log output.
				task->SetStatus(AsyncTask::Status::Running);
				task->Execute(th);

				if (auto context = th->GetContext()) {
					context->ExecuteAllAllocators(); //!< commandの実行
				}

				task->SetStatus(AsyncTask::Status::Completed);
				Logger::EngineThreadLog("[AsyncThread] task completed. tag: " + task->GetTag());
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
