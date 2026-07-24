#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* async
#include "AsyncExecutionTask.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

//* c++
#include <thread>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// Async namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Async {

	////////////////////////////////////////////////////////////////////////////////////////////
	// Thread class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief 特定のExecution種別を担当する1本のワーカースレッド. タスク取得ループを実行する
	class ExecutionThread {
	public:

		////////////////////////////////////////////////////////////////////////////////////////////
		// using
		////////////////////////////////////////////////////////////////////////////////////////////

		using GetTaskFunction = std::function<std::shared_ptr<ExecutionTask>(const ExecutionThread*)>;

		////////////////////////////////////////////////////////////////////////////////////////////
		// State enum class
		////////////////////////////////////////////////////////////////////////////////////////////
		//! @brief スレッドの状態(待機/実行中/終了)
		enum class State : uint8_t {
			Wait,
			Run,
			Terminate
		};

	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		ExecutionThread() = default;
		~ExecutionThread() { Shutdown(); }

		//! @brief スレッドを生成し, タスク取得関数を渡してループを開始する
		//! @param[in] execution 担当するExecution種別
		//! @param[in] main      タスクを取得する関数
		void Create(Execution execution, const GetTaskFunction& main);

		//* thread option *//

		//! @brief スレッドにデバッグ用の名前を設定する
		void SetName(const std::string& name);

		//! @brief 終了フラグを設定する
		void SetTerminate(bool isTerminate = true) { isTerminate_ = isTerminate; }

		//! @brief スレッドを終了・join する
		void Shutdown();

		//! @brief スレッドIDを取得する
		const std::thread::id GetId() const;

		//! @brief 現在のスレッド状態を取得する
		State GetState() const { return state_; }

		//! @brief 担当するExecution種別を取得する
		Execution GetExecution() const { return execution_; }

		//! @brief 終了が要求されているかを返す
		const bool IsTerminate() const { return isTerminate_; }

		//* operator [copy] (delete) *//

		ExecutionThread(const ExecutionThread&)            = delete;
		ExecutionThread& operator=(const ExecutionThread&) = delete;

		//* operator [move] *//

		ExecutionThread(ExecutionThread&&) noexcept            = default;
		ExecutionThread& operator=(ExecutionThread&&) noexcept = default;

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		//* parameter *//

		Execution execution_ = Execution::Cpu;

		//* thread *//

		std::thread thread_;
		bool isTerminate_ = false;

		State state_ = State::Wait;

		//* context *//

		std::unique_ptr<DirectXQueueContext> context_;

		//=========================================================================================
		// private variables
		//=========================================================================================

		static DirectXQueueContext::RenderQueue GetRenderQueue(Execution execution);

	};

}

SXAVENGER_ENGINE_NAMESPACE_END

