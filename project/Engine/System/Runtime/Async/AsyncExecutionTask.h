#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

//* c++
#include <cstdint>
#include <string>
#include <functional>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// Async namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Async {

	////////////////////////////////////////////////////////////////////////////////////////////
	// Execution enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Execution : uint8_t {
		Compute = 0,
		Copy    = 1,
		Cpu     = 2
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Task class
	////////////////////////////////////////////////////////////////////////////////////////////
	class ExecutionTask {
	public:

		////////////////////////////////////////////////////////////////////////////////////////////
		// Status enum class
		////////////////////////////////////////////////////////////////////////////////////////////
		enum class Status : uint8_t {
			Pending,   //!< 実行待ち
			Running,   //!< 実行中
			Completed, //!< 完了
		};

		////////////////////////////////////////////////////////////////////////////////////////////
		// using
		////////////////////////////////////////////////////////////////////////////////////////////

		using ExecutionFunction = std::function<void(const ExecutionTask*, const DirectXQueueContext*)>;

	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* execute option *//

		void Execute(const DirectXQueueContext* context) const;

		//! @brief タスクの実行関数を設定する
		//! @param execution タスクの実行方法.
		//! @param function タスクの実行関数. Execution::None以外の場合, 引数にDirectXQueueContextを受け取れる.
		void SetFunction(Execution execution, const ExecutionFunction& function);

		Execution GetExecution() const { return execution_; }

		//* status option *//

		const Status GetStatus() const { return status_; }

		//! @brief タスクが完了しているか確認する
		bool IsCompleted() const { return status_ == Status::Completed; }

		//! @brief 指定したステータスになるまで待機する
		void Wait(Status status = Status::Completed) const;

		void SetStatus(Status status) { status_ = status; }

		//* tag option *//

		const std::string& GetTag() const { return tag_; }

		void SetTag(const std::string& tag) { tag_ = tag; }

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		Status status_ = Status::Pending;

		ExecutionFunction function_ = nullptr;
		Execution execution_ = Execution::Cpu;

		std::string tag_ = "unknown execution task";

	};

}

SXAVENGER_ENGINE_NAMESPACE_END
