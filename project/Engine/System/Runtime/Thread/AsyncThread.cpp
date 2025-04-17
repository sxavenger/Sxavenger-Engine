#include "AsyncThread.h"

////////////////////////////////////////////////////////////////////////////////////////////
// AsyncThread class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AsyncThread::Create(AsyncExecution execution, const MainFunction& main, const ConditionFunction& condition) {

	execution_ = execution;

	main_      = main;
	condition_ = condition;

	thread_ = std::thread([this]() {
		EngineThreadLog("[AsyncThread]: begin thread.");
		while (!isTerminated_ && condition_()) {
			main_(this);
		}
		EngineThreadLog("[AsyncThread]: end thread.");
	});

	if (execution_ == AsyncExecution::None) {
		return;
	}

	// contextの作成
	context_ = std::make_unique<DirectXThreadContext>();

	D3D12_COMMAND_LIST_TYPE commandListType = GetCommandListType(execution_);
	uint32_t allocatorCount                 = GetAllocatorCount(execution_);

	context_->Init(allocatorCount, commandListType);
}

void AsyncThread::Shutdown() {
	SetTerminate();

	if (thread_.joinable()) {
		thread_.join();
	}

	context_.reset();
}

const DirectXThreadContext* AsyncThread::GetContext() const {
	Assert(context_ != nullptr, "thread type does not create context.");
	return context_.get();
}

D3D12_COMMAND_LIST_TYPE AsyncThread::GetCommandListType(AsyncExecution execution) {
	switch (execution) {
		case AsyncExecution::Copy:
			return D3D12_COMMAND_LIST_TYPE_COPY;

		case AsyncExecution::Compute:
			return D3D12_COMMAND_LIST_TYPE_COMPUTE;
	}

	Assert(false, "commandlist thread type error.");
	return D3D12_COMMAND_LIST_TYPE_NONE; //!< error case.
}

uint32_t AsyncThread::GetAllocatorCount(AsyncExecution execution) {
	switch (execution) {
		case AsyncExecution::Copy:
			return 1;

		case AsyncExecution::Compute:
			return 2;
	}

	Assert(false, "commandlist thread type error.");
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
					queue_.pop();
				}

				if (task->GetStatus() != AsyncTask::Status::Pending) {
					return;
				}

				// todo: thread log output.
				task->SetStatus(AsyncTask::Status::Running);
				task->Execute(th);
				task->SetStatus(AsyncTask::Status::Completed);
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

void AsyncThreadPool::SetTask(const std::shared_ptr<AsyncTask>& task) {
	if (task->GetStatus() != AsyncTask::Status::None) {
		return;
	}

	task->SetStatus(AsyncTask::Status::Pending);
	queue_.emplace(task);
	condition_.notify_one();
}
