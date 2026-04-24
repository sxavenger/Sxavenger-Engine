#include "AsyncExecutionThread.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/StreamLogger.h>

//* lib
#include <Lib/Adapter/Time/RunTimeTracker.h>
#include <Lib/Adapter/String/EncodedString.h>

//* external
#include <magic_enum.hpp>

//* windows
#include <processthreadsapi.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ExecutionThread class
////////////////////////////////////////////////////////////////////////////////////////////

void Async::ExecutionThread::Create(Execution execution, const GetTaskFunction& main) {

	//!< 引数の保存
	execution_ = execution;
	main_      = main;

	//!< threadの作成
	thread_ = std::thread([this]() {
		StreamLogger::EngineThreadLog(std::format("[Async::ExecutionThread<Execution::{}>] begin execution thread.", magic_enum::enum_name(execution_)));

		// main loop
		while (!isTerminate_) {
			std::shared_ptr<ExecutionTask> task = main_(this);

			if (task == nullptr) {
				continue; //!< taskがnullptrの場合はループの先頭に戻る.
			}

			if (task->GetStatus() != ExecutionTask::Status::Pending) {
				continue; //!< taskの状態が実行待ちでない場合はループの先頭に戻る.
			}

			//!< taskの実行
			state_ = State::Run;
			task->SetStatus(ExecutionTask::Status::Running);

			RunTimeTracker tracker;
			tracker.Begin();

			task->Execute(context_.get());

			tracker.End();

			task->SetStatus(ExecutionTask::Status::Completed);
			StreamLogger::EngineThreadLog(
				std::format("[Async::ExecutionThread<Execution::{}>] task completed. (time: {:.2f}s) tag: {}", magic_enum::enum_name(execution_), tracker.GetDeltaTime<TimeUnit::second>().time, task->GetTag())
			);

			state_ = State::Wait;
		}

		state_ = State::Terminate;
		StreamLogger::EngineThreadLog(std::format("[Async::ExecutionThread<Execution::{}>] terminate execution thread.", magic_enum::enum_name(execution_)));
	});

	if (execution_ != Execution::Cpu) {
		//!< contextの作成
		context_ = std::make_unique<DirectXQueueContext>();
		context_->Init(1, GetRenderQueue(execution_));

		std::wstringstream id;
		id << thread_.get_id();
		context_->SetName(std::format(L"Async ExecutionThread [id: {}]", id.str()));
	}

	ExecutionThread::SetName(std::format("ExecutionThread<Execution::{}>", magic_enum::enum_name(execution_)));
}

void Async::ExecutionThread::SetName(const std::string& name) {
	SetThreadDescription(thread_.native_handle(), EncodedString::Convert(name).c_str());
}

void Async::ExecutionThread::Shutdown() {
	SetTerminate();

	if (thread_.joinable()) {
		thread_.join();
	}

	context_.reset();
}

const std::thread::id Async::ExecutionThread::GetId() const {
	return thread_.get_id();
}

DirectXQueueContext::RenderQueue Async::ExecutionThread::GetRenderQueue(Execution execution) {
	switch (execution) {
		case Execution::Copy:
			return DirectXQueueContext::RenderQueue::Copy;

		case Execution::Compute:
			return DirectXQueueContext::RenderQueue::Compute;

		default:
			StreamLogger::Exception("thread execution type error.");
	}
}
